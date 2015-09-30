/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_api.h
 *
 * Description: declaration rgblight demo's API.
 *
 * History:
 * Version      Author          Date        Description
 *
 * 0.4.0        lxs           2015-09-08   Create file.
 *
 */
#ifndef _DEMO_RGBLIGHT_H_
#define _DEMO_RGBLIGHT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "wilddog.h"
#include "wilddog_demo_config.h"

/* USER config **/
#define RGBLIGHT_URL_APPID  YOUR_APPID//"zhinengcaideng"
#define RGBDEMO_DEVIDNAME  YOUR_DEVICE_NAME


#define RGBLIGHT_URL_HEAD "coap://"
#define RGBLIGHT_URL_DEVMANAGE    "devicemanage"
#define RGBLIGHT_URL_END  ".wilddogio.com"

#define BINDSERV_PORT (53364)
#define DEMO_FACTORYRESET_TIME	(5000)


#define LED_PWM_FREQ_HZ 2000
#define RGBDEMO_FLASHDATA_LEN   (32)
#define RGBDEMO_DEVID_LEN   (32)
#define RGBDEMO_USERID_LED  (32)
#define RGBDEMO_TOKEN_LEN   (256)
#define BINDSERVER_MAXBINTIME    (60)

#define DIFF(a,b)	(a>b)?(a-b):(b-a)


typedef struct BINDSERVER_FLASHDATA_T
{
    unsigned long bindflag;
    char pdevId[RGBDEMO_DEVID_LEN];
    char puserId[RGBDEMO_USERID_LED];  
    
    char ptoken[RGBDEMO_TOKEN_LEN];
}BindServer_FlashData_T;

typedef enum BINDSERVER_TIMESTATE_T
{
    BINDSERVER_TICKING,
    BINDSERVER_TIMEOUT
}BindServer_TimeState_T;

typedef enum BINDSERVER_STATE_T
{
    BINDSERVER_BIND_NODE,
    BINDSERVER_BIND_ING,
    BINDSERVER_BIND_BUILDTREE,
    BINDSERVER_BIND_SUCCESS,
}BindServer_State_T;
typedef enum LED_COLOR
{
    LED_COLOR_NONE,
    LED_COLOR_RED,
    LED_COLOR_ORANGE,
    LED_COLOR_YELLOW,
    LED_COLOR_GREEN,
    LED_COLOR_CYAN,
    LED_COLOR_BLUE,
    LED_COLOR_PURPLE,
    LED_COLOR_WHITE
}Led_Color_T;

typedef enum BLINK_TYPE
{
    BLINK_SOLID,
    BLINK_QUICK,
    BLINK_SLOW
}Blink_Type_T;

typedef enum WICED_CONNECT_STATE_T
{
   DEMO_WIFI_CONNECTED,
   DEMO_WIFI_DISCONNECTED,
   
}Wiced_Connect_State_T;




extern void demo_readFlash(char *pdevId,char *ptoken,
    char *puerId,unsigned long *pbindflag);
extern int demo_writeFlash(char *pdevId,char *puserId,
                          char *ptoken,unsigned long *pnewBindFlag);
extern int demo_creatUdpServer(int *fd);
extern BindServer_TimeState_T bindServer_getClickState(void);
extern int bindServer_buildTree(void);
extern BindServer_State_T bindServer_getState(void);
extern void bindServer_parseRecvPacket(int fd);
extern int bindServer_binding(void);

extern void demo_deviceInit(void);
extern void demo_deviceSet(int r, int g, int b,int iv);
extern void demo_parseDeviceTree(const Wilddog_Node_T* p_snapshot);
extern void demo_device_thread(void);
extern void rgbLed_setLedstatus(Led_Color_T color, Blink_Type_T type);
extern Wilddog_Node_T *demo_buildDevicesTree(void);
extern int start_smartconfig(void);

extern void demo_factoryRestState_thread( void );


#ifdef __cplusplus
}
#endif

#endif /*_DEMO_RGBLIGHT_H_*/

