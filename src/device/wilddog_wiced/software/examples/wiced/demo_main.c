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
#include "wiced.h"
#include "wiced_tcpip.h"
#include "wifi_config_dct.h"
#include "wd_sdk_dct.h"
#include "wilddog_demo_config.h"
#include "wilddog.h"
#include "wilddog_debug.h"
#include "wilddog_port.h"

#include "demo_api.h"
#include "demo_platform.h"


#define RGBMAIN_LED_THREAD_STACK_SIZE       (1*1024)
#define RGBMAIN_KEY_THREAD_STACK_SIZE       (1*1024)
#define RGBMAIN_SUBSCRIBE_THREAD_STACK_SIZE       (5*1024)

static u8 l_key_pressed = FALSE;

static wiced_thread_t    RGBMAIN_LedThread;
static wiced_thread_t    RGBMAIN_KeyThread;
static wiced_thread_t    RGBMAIN_subscribeThread;


#define RGBLIGHT_LED_THREAD_NAME    "LED_MAIN"
#define RGBLIGHT_KEY_THREAD_NAME    "KEY_MAIN"
#define RGBLIGHT_SUBSCRIBE_THREAD_NAME   "SUBSCRIBE_MAIN"

typedef enum RGBMAIN_STATE_T
{
    RGBMAIN_POWERUP,
    RGBMAIN_SMNT,
    RGBMAIN_CONNECTING,
    RGBMAIN_BINDING,
    RGBMAIN_SUBSCIBE,

    RGBMAIN_BIND_TIMEOUT,
}Rgbmain_State_T;

STATIC Rgbmain_State_T l_rgbmain_state = RGBMAIN_POWERUP;


STATIC void demo_main_printfState(void)
{
    switch(l_rgbmain_state)
    {
        case RGBMAIN_POWERUP:
                printf("\tpowerup..\n");
                break;
        case RGBMAIN_CONNECTING:
                printf("\t connecting to the net..\n");
                break;
        case RGBMAIN_SMNT:
                printf("\t smart config ...\n");
                break;
        case RGBMAIN_BINDING:
                printf("\t binding ...\n");
                break;
        case RGBMAIN_SUBSCIBE:
                printf("\t observering ...\n");
                break;
        case RGBMAIN_BIND_TIMEOUT:
                printf("\tRGBMAIN_BIND_TIMEOUT .. \n");
                break;
    }
}
STATIC void demo_main_setDevice(void)
{
    switch(l_rgbmain_state)
    {
        case RGBMAIN_POWERUP:
            rgbLed_setLedstatus(LED_COLOR_RED, BLINK_SOLID);
            break;
        case RGBMAIN_SMNT:
            rgbLed_setLedstatus(LED_COLOR_RED, BLINK_QUICK);
            break;
        case RGBMAIN_CONNECTING:
            rgbLed_setLedstatus(LED_COLOR_RED, BLINK_QUICK);
            break;
        case RGBMAIN_BINDING:
            rgbLed_setLedstatus(LED_COLOR_BLUE, BLINK_QUICK);
            break;
        case RGBMAIN_SUBSCIBE:
            //rgbLed_setLedstatus(LED_COLOR_RED, BLINK_SOLID);
            break;
        case RGBMAIN_BIND_TIMEOUT:
            rgbLed_setLedstatus(LED_COLOR_BLUE, BLINK_SLOW);
            break;
   
    }
}
STATIC void demo_main_setState(Rgbmain_State_T newstate)
{
    l_rgbmain_state = newstate;
    demo_main_setDevice();
    demo_main_printfState();
}
STATIC Rgbmain_State_T demo_main_getState(void)
{
    return l_rgbmain_state;
}
/*
 * Function:    demo_main_getRouterConnectState .
 * Description:  try to connect to ap.
 * Input:   N.
 * Output: N.      
 * Return:  Wiced_Connect_State_T . 
*/
STATIC Wiced_Connect_State_T demo_main_getRouterConnectState(void)
{
    wiced_result_t result;
    int ssidLen = 0,connect_retryCnt = 3;
    user_dct_data_t* dct= NULL;
    wiced_config_ap_entry_t* dct_ap_entry = NULL;

    
    /* if it have ssid and password. */
    wiced_dct_read_lock( (void**) &dct_ap_entry,\
        WICED_TRUE, DCT_WIFI_CONFIG_SECTION,\
        offsetof(platform_dct_wifi_config_t, stored_ap_list),\
        sizeof(wiced_config_ap_entry_t) );
    WPRINT_APP_INFO(("SSID len:%d\n",dct_ap_entry->details.SSID.length));
    WPRINT_APP_INFO(("SSID val:%s\n",dct_ap_entry->details.SSID.value));

    ssidLen = dct_ap_entry->details.SSID.length;
    wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );
    if(ssidLen == 0)
        return DEMO_WIFI_DISCONNECTED;

    /* try connect to ap up to 3 times .*/
    while (connect_retryCnt--) {
        WPRINT_APP_INFO(("in up_tries = %d\n",connect_retryCnt));
        result = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
        if (result == WICED_SUCCESS) {
            return DEMO_WIFI_CONNECTED;
        } else {
            WPRINT_APP_INFO(("Network up failed, try again (%d left)  error code :%d\n", connect_retryCnt, result));
        }
    }

    return DEMO_WIFI_DISCONNECTED;
}
/*
 * Function:    demo_main_connectRouter .
 * Description: Connection Routing.
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/
STATIC void demo_main_connectRouter(void)
{
    int res = 0;
    demo_main_setState(RGBMAIN_CONNECTING);
    if( DEMO_WIFI_CONNECTED != demo_main_getRouterConnectState())
    {
        
        demo_main_setState(RGBMAIN_SMNT);
        res = start_smartconfig();

        if(WICED_SUCCESS != res)
        {
            while(1)
            {
                 demo_delay_milliseconds(1000);
            }
        }

    }
    else
        WPRINT_APP_INFO( ("Network up success!\n") );
    
}
/*
 * Function:    demo_main_subscribe_thread .
 * Description: binding and subscribe thread .
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/
STATIC void demo_main_subscribe_thread(VOID)
{

       if(bindServer_binding() <0)
       {
            demo_main_setState(RGBMAIN_BIND_TIMEOUT);
            return ;
       }
       else
            demo_main_setState(RGBMAIN_SUBSCIBE); 
       
       if(demo_main_getState() == RGBMAIN_SUBSCIBE )
       {
           wiced_rtos_delete_thread(&RGBMAIN_LedThread);
           demo_deviceSet(0,0,0,0);
           demo_subscribe();
        }
}
#if 1
STATIC void rgbMain_getMac(void)
{
    char device_id[256];
    wiced_mac_t* dct_mac;
    
    memset(device_id,0,256);
    wiced_dct_read_lock( (void**) &dct_mac,\
       WICED_TRUE, DCT_WIFI_CONFIG_SECTION,\
       offsetof(platform_dct_wifi_config_t, mac_address),\
       sizeof(wiced_mac_t) );
    sprintf(device_id,"%02x:%02x:%02x",dct_mac->octet[3], dct_mac->octet[4], dct_mac->octet[5]);
    WPRINT_APP_INFO(("device_id :%s\n",device_id));
    wiced_dct_read_unlock( dct_mac, WICED_TRUE );
}
#endif

/*
 * Function:    demo_main_init .
 * Description: init sysy pereform and devices .
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/

STATIC void demo_main_init(void)
{
    
    demo_factoryRestInit();
    /* led init.*/
    demo_deviceInit();
    demo_main_setState(RGBMAIN_POWERUP);

}

/**
 *  Application start .
 */
void application_start( void )
{
    wiced_result_t result;

    wiced_init();
    rgbMain_getMac();
    /*init*/
    demo_main_init();
    /* creat thread*/
    wiced_rtos_create_thread(&RGBMAIN_LedThread,               \
                              WICED_APPLICATION_PRIORITY,\
                              RGBLIGHT_LED_THREAD_NAME,        \
                              &demo_device_thread,           \
                              RGBMAIN_LED_THREAD_STACK_SIZE,(void*)NULL);

    wiced_rtos_create_thread(&RGBMAIN_KeyThread,               \
                              WICED_APPLICATION_PRIORITY,\
                              RGBLIGHT_KEY_THREAD_NAME,        \
                              &demo_factoryRestState_thread,           \
                              RGBMAIN_KEY_THREAD_STACK_SIZE,(void*)NULL );
    /* connect to ap .*/
    demo_main_connectRouter();

    wiced_rtos_create_thread(&RGBMAIN_subscribeThread,               \
                                  WICED_DEFAULT_WORKER_PRIORITY,\
                                  RGBLIGHT_SUBSCRIBE_THREAD_NAME,        \
                                  &demo_main_subscribe_thread,           \
                                  RGBMAIN_SUBSCRIBE_THREAD_STACK_SIZE,(void*)NULL );
}
#if 0

int main(int argc, char **argv)
{
    int fd;
    
    if(argc <= 1)
    {
        printf("input your device id\n");
        return -1;
    }
    else
        printf("your device id :%s\n",argv[1]);

    demo_deviceInit();
    demo_main_setState(RGBMAIN_CONNECTING); 
    demo_writeFlash(argv[1],NULL,NULL,NULL);
    rgbFlash_writeDevId(argv[1]);
    l_rgbmain_state = RGBMAIN_BINDING;
    
    demo_main_setState(RGBMAIN_BINDING);    
    demo_creatUdpServer(&fd);
    while(1)
    {
        bindServer_parseRecvPacket(fd);
        bindServer_buildTree();
        if(bindServer_getState() == BINDSERVER_BIND_SUCCESS )
        {
            
            demo_main_setState(RGBMAIN_SUBSCIBE);   
            demo_destoryUDPServer(fd);
            pritnf("\t Bind Successfully \n");
            break;
        }
        if( bindServer_getClickState() == BINDSERVER_TIMEOUT )
        {
            printf("Binserver time out  \n");
            demo_destoryUDPServer(fd);
            break;
        }
    }
    if(demo_main_getState() == RGBMAIN_SUBSCIBE)
        demo_subscribe();
    
    return 0;
}
#endif /* WILDDOG_PORT_TYPE_WICED */

