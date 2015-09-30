/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_rgbFlash.c
 *
 * Description: API of read and write sys data to Flash.
 *
 * History:
 * Version      Author          Date        Description
 *
 * 0.4.0        lxs              2015-09-8  Create file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wilddog.h"

#include "demo_api.h"
#ifdef WILDDOG_PORT_TYPE_WICED

#include "wiced.h"
#include "wiced_tcpip.h"
#include "wifi_config_dct.h"
#include "wd_sdk_dct.h"
#endif

STATIC BindServer_FlashData_T flash_data;

void demo_readFlash(char *pdevId,char *ptoken,
    char *puserId ,unsigned long *pbindflag)
{
#ifndef WILDDOG_PORT_TYPE_WICED

    if(pdevId)
        memcpy(pdevId,flash_data.pdevId,strlen(flash_data.pdevId));
    if(ptoken)
        memcpy(ptoken,flash_data.ptoken,strlen(flash_data.ptoken));
    if(puserId)
        memcpy(puserId,flash_data.puserId,strlen(flash_data.puserId));
    if(pbindflag)
        *pbindflag = flash_data.bindflag;
#else
    user_dct_data_t* dct;

    wiced_dct_read_lock( (void**) &dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
    
    WPRINT_APP_INFO(("device_id:%s\n", dct->device_id));
    WPRINT_APP_INFO(("user_id:%s\n", dct->user_id));
    WPRINT_APP_INFO(("isbind:%u\n", dct->isBinded));


    if(pdevId)
        memcpy( pdevId, dct->device_id, strlen(dct->device_id));
    if(puserId)
        memcpy( puserId, dct->user_id, strlen(dct->user_id));
    if(ptoken)
        memcpy(ptoken,flash_data.ptoken,strlen(flash_data.ptoken));
    if(pbindflag)
        *pbindflag = dct->isBinded;

    wiced_dct_read_unlock( dct, WICED_TRUE ); 

#endif
}
int demo_writeFlash(char *pdevId,char *puserId,
                                     char *ptoken,unsigned long *pnewBindFlag)
{
#ifndef WILDDOG_PORT_TYPE_WICED
    memset(flash_data.pdevId,0,sizeof(flash_data.pdevId));  
    memset(flash_data.ptoken,0,sizeof(flash_data.ptoken));    
    memset(flash_data.puserId,0,sizeof(flash_data.puserId));

    if(pdevId)
        memcpy(flash_data.pdevId,pdevId,strlen(pdevId));  
    if(puserId)
        memcpy(flash_data.puserId,puserId,strlen(puserId));
    if(ptoken)
        memcpy(flash_data.ptoken,ptoken,strlen(ptoken));
    if(pnewBindFlag)
        flash_data.bindflag = *pnewBindFlag;
#else
    user_dct_data_t* dct;
     
    wiced_dct_read_lock( (void**) &dct,\
        WICED_TRUE, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );

    if(pdevId)
       {
            memset(dct->device_id,0,RGBDEMO_DEVID_LEN);
            memcpy(dct->device_id, pdevId, RGBDEMO_DEVID_LEN);
            
        }   
    if(puserId)
    {    
        memset(dct->user_id,0,RGBDEMO_USERID_LED);
        memcpy(dct->user_id,puserId,RGBDEMO_USERID_LED);
    }
    if(ptoken)
    {
        memset(flash_data.ptoken,0,RGBDEMO_TOKEN_LEN);
        memcpy(flash_data.ptoken,ptoken,strlen(ptoken));
    }    
    if(pnewBindFlag)
        dct->isBinded = *pnewBindFlag;
    wiced_dct_write( (const void*) dct, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
    wiced_dct_read_unlock( dct, WICED_TRUE );
#endif
    return 0;
}


