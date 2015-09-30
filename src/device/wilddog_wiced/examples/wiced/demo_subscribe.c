/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_rgbLightMain.c
 *
 * Description: rgb light main control flow .
 *
 * History:
 * Version      Author          Date        Description
 *
 * 0.4.0        lxs           2015-09-08   Create file.
 *
 */

#include <stdio.h>
#include <string.h>
#include "wilddog_demo_config.h"
#include "wilddog.h"
#include "wilddog_debug.h"
#include "wilddog_port.h"

#include "demo_api.h"
#include "demo_platform.h"

/* main state*/
/* demo_rgbmain.c*/
STATIC void demo_subscribe_addobserverFunc
    (
    const Wilddog_Node_T* p_snapshot,
    void* arg,
    Wilddog_Return_T err
    )
{
    if(p_snapshot)
    {
        wilddog_debug_printnode(p_snapshot);
        printf("\n");
        demo_parseDeviceTree(p_snapshot);
     }
}

STATIC void demo_subscribe_authFunc(void* arg, Wilddog_Return_T err)
{
   
    printf("auth error:%d\n", err);
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("setValue error : %d!",err);
        return;
    }

    return;
}

int demo_subscribe(void)
{
    Wilddog_T wd_client=0;
    int res =0;
    char purl[256];
    char pdevId[RGBDEMO_DEVID_LEN];
    char ptoken[RGBDEMO_TOKEN_LEN];
    char puserId[RGBDEMO_USERID_LED];

    static int test_cnt = 0;
    
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
    
    wd_client = wilddog_initWithUrl((Wilddog_Str_T *)purl);
    if(wd_client == 0)
    {
        return -1;
    }
     /* set auth*/    
    memset(purl,0,sizeof(purl));
    sprintf(purl,"%s%s",RGBLIGHT_URL_APPID,RGBLIGHT_URL_END);
    res = wilddog_auth((Wilddog_Str_T*)purl,(u8*)ptoken,\
        strlen(ptoken), demo_subscribe_authFunc, NULL);
    if(res < 0 )
        goto SUBSCIBE_ERROR;
    res = wilddog_addObserver(wd_client, WD_ET_VALUECHANGE, \
			 	demo_subscribe_addobserverFunc, NULL);
    if(res < 0)
    {
        printf("addObserver error : %d\n",res);
        goto SUBSCIBE_ERROR;
    }
    while(1)
    {
        wilddog_trySync();
        demo_delay_milliseconds(500);
        wilddog_increaseTime(500);
        if(++test_cnt > 10)
        {
            test_cnt = 0;
            wilddog_debug();
        }
    }
    
SUBSCIBE_ERROR:
    
    wilddog_destroy(&wd_client);
    return  res;
}







