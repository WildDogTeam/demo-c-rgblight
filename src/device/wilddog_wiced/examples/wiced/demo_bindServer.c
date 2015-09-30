/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_bindServer.c
 *
 * Description: bind server's API.
 *
 * History:
 * Version      Author          Date        Description
 *
 * 0.4.0        lxs           2015-09-08   Create file.
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "wilddog.h"
#include "wilddog_debug.h"
#include "wilddog_port.h"

#include "demo_platform.h"
#include "demo_api.h"
#define DIFF(a,b)   ((a>b)?(a-b):(b-a))

//#define BINDSERVER_HOST "zhinengcaideng.wilddogio.com"


#define APPMSG_CMD  "cmd"
#define APPMSG_CMD_GET  "get"
#define APPMSG_CMD_SET  "set"
#define APPMSG_CMD_ACK  "ack"
#define APPMSG_SUBCMD   "subcmd"
#define APPMSG_SUBCMD_DEVID "devId"
#define APPMSG_SUBCMD_TOKEN "token"
#define APPMSG_SUBCMD_USERID    "userId"
#define APPMSG_CMD_ACKSET \
    "{\"cmd\": \"ack\",\"subcmd\": {\"token\": \"success\",\"userId\": \"success\"}}"



#define BINDSERVER_MAXLINE 512


typedef enum BINDSERVER_SETSTATE_T
{
    BINDSERVER_SET_ING,
    BINDSERVER_SET_ERROR,
    BINDSERVER_SET_SUCCESS,
}BindServer_SetState_T;

STATIC BindServer_State_T l_binServer_state = BINDSERVER_BIND_NODE;
STATIC u32 l_binServer_time_cnt = 0;

typedef enum BINDSERVER_RECVTYPE_T
{
    BINDSERVER_RECV_UNKNOWN,
    BINDSERVER_RECV_GETDEV,
    BINDSERVER_RECV_SETTOKEN,
    
}BindServer_RecvType_T;
typedef enum RGBMAIN_BINDSTATE_T
{   
    RGBMIN_NO_BIND,
    RGBMIN_HAVE_BIND,
}Rgbmain_Bindstate_T;


char* strstr(const char *s1,const char *s2)
{
	const char *p=s1;
	const size_t len=strlen(s2);
	for(;(p=strchr(p,*s2))!=0;p++)
	{
		if(strncmp(p,s2,len)==0)
		return	(char*)p;
	}
return(NULL);
}

static const char *skip_space(const char *str)
{
	while(*str == ' ' || *str == '\t')
	{
		str++;
	}
	return str;
}
/*
* Function:    sjson_get_value
* Description: get json value
* Input:      input :json ; name: item's key ; maxlen : input buf sizeof
* Output:     output : value  ;
*                 *maxlen :  value's len
* Return:     value's len
*/
int sjson_get_value(const char *input, const char *name,\
                        char *output, int *maxlen)
{
	const char *p_name = strstr(input, name);
	const char *p;
	char end;
	int i;
	int len;
	if(p_name == NULL)
		goto err1;

	end = '\"';
	p = strstr(p_name + 1, ":\"");
	if(p)
	{
		p += 2;
	}
	else
	{
		p = strstr(p_name + 1, ":");
		if(p == NULL)
			goto err1;
		p += 1;
		p = skip_space(p);
		if(*p == '{')
		{
			end = '}';
		}
		else
		if(*p == '\"')
		{
			p += 1;
			//end = '}';//?
			end = '\"';
		}
		else
		if(*p == '[')
		{
			end = ']';
		}
		else
		{
			end = ',';
		}
	}

	memset(output, 0, *maxlen);
	len = 0;
	for(i = 0; i < *maxlen; i++)
	{
		while(*p == '\\')
		{
			p++;
		}
		if(*p == end)
		{
			if(end == '}')
			{
				output[i] = *p;
				break;
			}
			else
			{
				break;
			}
		}
		output[i] = *p;
		p++;
		len++;
	}
	*maxlen = len;
	return len;
err1:
	return -1;
}
/*
 * Function:    bindServer_clickStart .
 * Description: star count time while build  bin server.
 * Input:       N  
 * Output:     N.
 * Return:      N.
*/
void bindServer_clickStart(void)
{

    l_binServer_time_cnt = demo_getSysTime_sec();
}

/*
 * Function:    bindServer_getClickState.
 * Description: jude where bin server time out.
 * Input:    N.  
 * Output:  N.
 * Return:  BinServer_TimeState_T .
*/
BindServer_TimeState_T bindServer_getClickState(void)
{
    u32 currtime = 0;
    currtime = demo_getSysTime_sec();
    if( DIFF(currtime,l_binServer_time_cnt) > BINDSERVER_MAXBINTIME)
        return BINDSERVER_TIMEOUT;
    else
        return BINDSERVER_TICKING;
}
STATIC void bindServer_setState(BindServer_State_T newState)
{
    l_binServer_state = newState ;
}
BindServer_State_T bindServer_getState(void)
{
    return l_binServer_state;
}

/*
 * Function:    bindServer_recvParse .
 * Description: parse received packet .
 * Input:    recvdata :received packet .
 * Output:       
 * Return:  BINDSERVER_RECVTYPE_T . 
*/
BindServer_RecvType_T bindServer_getRecvType(const char *recvdata)
{
   char p_value[RGBDEMO_FLASHDATA_LEN];
   int len = RGBDEMO_FLASHDATA_LEN;
   
   memset(p_value,0,RGBDEMO_FLASHDATA_LEN);
   if(sjson_get_value(recvdata,APPMSG_CMD,p_value,&len) < 0 )   
        return BINDSERVER_RECV_UNKNOWN;
   
   printf("cmd value : %s \n",p_value);
   if( 0 == strncmp(APPMSG_CMD_GET,p_value,strlen(APPMSG_CMD_GET)))
        return BINDSERVER_RECV_GETDEV;
   else
   if( 0 == strncmp(APPMSG_CMD_SET,p_value,strlen(APPMSG_CMD_SET)))
        return BINDSERVER_RECV_SETTOKEN;
   
   return BINDSERVER_RECV_UNKNOWN;
}
/*
* Function:    bindServer_storeUerIdToken
* Description:  store userId and token to flash or other nonvolatile storage
* Input:    recvdata :received packet
* Output:       
* Return:  <0 mean fault
*/
 STATIC int bindServer_storeUerIdToken(const char *recvdata)
{
    char puserId[RGBDEMO_FLASHDATA_LEN];
    char ptoken[RGBDEMO_TOKEN_LEN];
    int userIdLen = RGBDEMO_FLASHDATA_LEN;
    int tokenLen = RGBDEMO_TOKEN_LEN;

    memset(puserId,0,RGBDEMO_FLASHDATA_LEN);    
    memset(ptoken,0,RGBDEMO_TOKEN_LEN);

    /* get userid and token's value, check if they exit */
    if((sjson_get_value(recvdata,APPMSG_SUBCMD_USERID,puserId,&userIdLen) < 0 ) ||
       (sjson_get_value(recvdata,APPMSG_SUBCMD_TOKEN,ptoken,&tokenLen) < 0 ))
       return -1;
    
    printf("store userid : %s ; token : %s \n",puserId,ptoken);
    
    return demo_writeFlash(NULL,puserId,ptoken,NULL);
}

STATIC void bindServer_buildtree_authFunc(void* arg, Wilddog_Return_T err)
{
   
    printf("auth error:%d\n", err);
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("setValue error : %d!",err);
        return;
    }

    return;
}

/*
* Function:     bindServer__buildtree_setValueFunc
* Description:  set auth call back function
* Input:     err : http error code 
* Output:   arg : BINDSERVER_SET_SUCCESS mean tree have beed upload to server success.
* Return:   N
*/
STATIC void bindServer_buildtree_setValueFunc(void* arg, Wilddog_Return_T err)
{

    printf("setValue error:%d\n", err);
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("setValue error = %d!",err);
        *(char*)arg = BINDSERVER_SET_ERROR;
        return;
    }
    else
        *(char*)arg = BINDSERVER_SET_SUCCESS;

    return;
}

/*
* Function:     bindServer_buildTree
* Description: build the device's tree and upload to the server
* Input:     N
* Output:   N    
* Return:    
*/
int bindServer_buildTree(void)
{
    
    Wilddog_T wdBuildTreeClient = 0;
    char setState = BINDSERVER_SET_ING;
    char purl[256];
    Wilddog_Node_T *p_head = NULL;
    
    char pdevId[RGBDEMO_DEVID_LEN];
    char ptoken[RGBDEMO_TOKEN_LEN];
    char puserId[RGBDEMO_USERID_LED];

    if(bindServer_getState()!= BINDSERVER_BIND_ING)
        return -1;
    
    memset(ptoken,0,RGBDEMO_TOKEN_LEN);
    memset(puserId,0,RGBDEMO_USERID_LED);
    memset(pdevId,0,RGBDEMO_DEVID_LEN);
    memset(purl,0,256);
    
    /* get userid and token */
    demo_readFlash(pdevId,ptoken,puserId,NULL);

    /* init wilddog client */
    sprintf(purl,"%s%s%s/%s/%s/%s",RGBLIGHT_URL_HEAD,\
            RGBLIGHT_URL_APPID,RGBLIGHT_URL_END,\
            RGBLIGHT_URL_DEVMANAGE,puserId,pdevId);
    printf("build tree url : %s\n",purl);
    printf("build tree token : %s ; uerId : %s\n",ptoken,puserId);
    
    wdBuildTreeClient = wilddog_initWithUrl((Wilddog_Str_T *)purl);
    if(!wdBuildTreeClient)
    {
        wilddog_debug("new wilddog failed!");
        return -1;
    }
    
    /* creat device tree */
    p_head = demo_buildDevicesTree();
    if( p_head == NULL)
        return -1;
    
    /* set auth*/    
    memset(purl,0,sizeof(purl));
    sprintf(purl,"%s%s",RGBLIGHT_URL_APPID,RGBLIGHT_URL_END);
  
    wilddog_auth((Wilddog_Str_T*)purl,(u8*)ptoken,\
        strlen(ptoken), bindServer_buildtree_authFunc, NULL);
    
    /* uploading tree to the server */
    wilddog_setValue(wdBuildTreeClient, p_head,\
        bindServer_buildtree_setValueFunc,(void*)&setState);
    
    

    while(1)
     {
        wilddog_trySync();
        if( setState != BINDSERVER_SET_ING )
        {
            if( setState == BINDSERVER_SET_SUCCESS )
                bindServer_setState(BINDSERVER_BIND_BUILDTREE);
            
            break;
        }
        wiced_rtos_delay_milliseconds(10);
        wilddog_increaseTime(110);
    }

    printf("destory tree \n");
    wilddog_node_delete(p_head);
    wilddog_destroy(&wdBuildTreeClient);
   
   return 0;
}

/*
* Function:    bindServer_ackDevId .
* Description:  ack App with devId .
* Input:     fd: socket; psendbuf : buf that will store  content to be send
*              pcliendaddr : send to address .
* Output:   N    
* Return:   sendto result . 
*/
STATIC int bindServer_ackDevId(int fd,char *psendbuf)
{
    char pdevid[RGBDEMO_FLASHDATA_LEN];
         
    memset(pdevid,0,RGBDEMO_FLASHDATA_LEN);

    demo_readFlash(pdevid,NULL,NULL,NULL);
    sprintf(psendbuf,"%s%s%s", "{\"cmd\": \"ack\",\"subcmd\": {\"devId\": \"",\
           pdevid, "\"}}");
    printf("ACK app with mesg : %s\n",psendbuf);

    return demo_sendUDPPacket(fd,psendbuf,strlen(psendbuf));   
}
STATIC int bindServer_ackBinSuccess(int fd,char *psendbuf)
{
    int len = 0;
    memcpy(psendbuf,APPMSG_CMD_ACKSET,strlen(APPMSG_CMD_ACKSET));
    printf("Ack with mesg : %s\n",psendbuf);

    len =  demo_sendUDPPacket(fd,psendbuf,strlen(psendbuf));
    if( len != strlen(psendbuf))
        return -1;
    else
        return 0;
}

/*
* Function:    bindServer_recvParse
* Description: parse received packet
* Input:    recvdata :received packet
* Output:       
* Return:  BINDSERVER_RECVTYPE_T 
*/
void bindServer_parseRecvPacket(int fd)
{

    char mesg[BINDSERVER_MAXLINE];
    u32 mesgLen = BINDSERVER_MAXLINE,recvlen = 0;

   /* waiting for receive data */
    memset(mesg,0,BINDSERVER_MAXLINE);
    if(bindServer_getState() == BINDSERVER_BIND_ING)
        return;
    recvlen = demo_recv(fd,mesg,&mesgLen);
    if(recvlen > 0)
    {
        if( bindServer_getState() == BINDSERVER_BIND_NODE &&
            bindServer_getRecvType(mesg) == BINDSERVER_RECV_GETDEV)
        {
           memset(mesg,0,BINDSERVER_MAXLINE);
           bindServer_ackDevId(fd,mesg);
        }
        else
        if(bindServer_getRecvType(mesg) == BINDSERVER_RECV_SETTOKEN)
        {
            if(bindServer_getState() == BINDSERVER_BIND_NODE)
            {
                /* store token and userId . */
                bindServer_storeUerIdToken(mesg);
                
                bindServer_setState(BINDSERVER_BIND_ING);
             }
            else 
            if( bindServer_getState() == BINDSERVER_BIND_BUILDTREE )
            {
                /* After build tree and upload to the server, then response the App bin success.*/
                memset(mesg,0,BINDSERVER_MAXLINE);
                if(bindServer_ackBinSuccess(fd,mesg) == 0)
                    bindServer_setState(BINDSERVER_BIND_SUCCESS);
            } 
        }
    }

}

STATIC Rgbmain_Bindstate_T bindServer_getBindstate(void)
{
    unsigned long bindstate = 0;
    demo_readFlash(NULL,NULL,NULL,&bindstate);
    if( bindstate == RGBMIN_HAVE_BIND)
    {
        return RGBMIN_HAVE_BIND;
        }
    else
        return RGBMIN_NO_BIND;
}
int bindServer_binding(void)
{
    int fd =0,res = 0;
    
    
    if( bindServer_getBindstate() != RGBMIN_NO_BIND )
        return 0;
    
    demo_creatUdpServer(&fd);
    bindServer_clickStart();
    while(1)
    {
        bindServer_buildTree();
       if(bindServer_getState() == BINDSERVER_BIND_SUCCESS )
       {
           unsigned long newBindFlag = RGBMIN_HAVE_BIND;
           demo_destoryUDPServer(fd);
           wilddog_debug();
           /*set bind flag .*/
           demo_writeFlash(NULL,NULL,NULL,&newBindFlag);
           printf("\t Bind Successfully \n");
           res = 0; 
           break;
       }
       if( bindServer_getClickState() == BINDSERVER_TIMEOUT )
       {
       
           printf("Binserver time out  \n");
           demo_destoryUDPServer(fd);
           res = -1;
           break;
       }
       
       demo_delay_milliseconds(50);
    }
    return res;
}


