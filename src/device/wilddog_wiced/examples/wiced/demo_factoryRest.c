
/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_doFactoryRest.c
 *
 * Description: binServer API .
 *
 * History:
 * Version      Author          Date        Description
 *
 * 0.4.0        lxs           2015-09-08   Create file.
 *
 */
 #include <stdio.h>
 
#include "wilddog.h"
#include "wilddog_debug.h"
#include "demo_api.h"
#include "demo_platform.h"
#ifdef WILDDOG_PORT_TYPE_WICED
#include "wifi_config_dct.h"
#include "wd_sdk_dct.h"

#endif
typedef enum RGBMAIN_FACTORYRESTSTATE_T
{
    RGBMIN_NO_FACTORYREST,
    RGBMIN_DO_FACTORYREST
}Rgbmain_Factoryreststate_T;

static u8 l_key_pressed = FALSE;

STATIC void demo_factoryRestTriggerStart(void);

void demo_factoryRestInit(void)
{
#ifdef WILDDOG_PORT_TYPE_WICED

    /* factory key init.*/
	wiced_gpio_init(DEMO_KEY_GPIO,INPUT_PULL_UP );
	wiced_gpio_input_irq_enable(DEMO_KEY_GPIO,\
		IRQ_TRIGGER_FALLING_EDGE, demo_factoryRestTriggerStart, NULL);
#endif
}
void demo_factoryRestRouter(void)
{
#ifdef WILDDOG_PORT_TYPE_WICED

    wiced_config_ap_entry_t* dct_ap_entry;
    user_dct_data_t* dct;

    wiced_dct_read_lock( (void**) &dct_ap_entry, WICED_TRUE, \
        DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list),\
        sizeof(wiced_config_ap_entry_t) );
    memset(dct_ap_entry, 0, sizeof(wiced_config_ap_entry_t));
    wiced_dct_write( (const void*) dct_ap_entry, DCT_WIFI_CONFIG_SECTION, \
        offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
    wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );
#endif

}

int demo_doFactoryRest(void)
{
    char pdevId[RGBDEMO_DEVID_LEN];
    char puserId[RGBDEMO_USERID_LED];
    char ptoken[RGBDEMO_TOKEN_LEN];
    unsigned long bindFlag =0;

    demo_factoryRestRouter();
    
    memset(pdevId,0,RGBDEMO_DEVID_LEN);   
    memset(puserId,0,RGBDEMO_USERID_LED);    
    memset(ptoken,0,RGBDEMO_TOKEN_LEN);
    memcpy(pdevId,RGBDEMO_DEVIDNAME,strlen(RGBDEMO_DEVIDNAME));

    return demo_writeFlash(pdevId,puserId,ptoken,&bindFlag);
}
 
/*
 * Function:    demo_factoryRestTriggerStart .
 * Description: interruption call while key's gpio falling from 1 to 0  .
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/
STATIC void demo_factoryRestTriggerStart(void)
{
	wilddog_debug();
	l_key_pressed = TRUE;	
}
/*
 * Function:    rgbMain_factoryKeyDetect.
 * Description:  computing the time of factory key was pressed,restore factory settings while it larger 5s.
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/
STATIC Rgbmain_Factoryreststate_T demo_getFactoryRestState( void )
{
	
	static u32 l_keypress_time = 0;
	u32 diff_temp = 0,curr_tm = 0;
	
	 
	curr_tm = demo_getSysTime_ms();

	if( FALSE == l_key_pressed)
		return RGBMIN_NO_FACTORYREST;
	
	if( l_keypress_time == 0 )
	{
		if( WICEDPIN_LOW == demo_getPinLeve(DEMO_KEY_GPIO))
		{
		
			l_keypress_time = curr_tm; 
		}
	}
	else
	{
		diff_temp =(u32) DIFF(curr_tm,l_keypress_time);
        
		if(  diff_temp >= DEMO_FACTORYRESET_TIME)
		{
			printf("11>>>curr =%d;press_tm = %d,diff = %lu \n",curr_tm,l_keypress_time, \
					 diff_temp);
			l_key_pressed = FALSE;
			l_keypress_time = 0;
			printf(">>>>FACTORY_RESET_FLAG \n");
            return RGBMIN_DO_FACTORYREST;
		}
		else
		if( WICEDPIN_HIGHT == demo_getPinLeve(DEMO_KEY_GPIO) )
		{
		
            l_key_pressed = FALSE;
			l_keypress_time = 0;
		}
	}
    return RGBMIN_NO_FACTORYREST;
}
/*
 * Function:    demo_factoryRestState_thread .
 * Description:  computing the time of factory key was pressed,restore factory settings while it larger 5s.
 * Input:   N.
 * Output: N.      
 * Return:  N . 
*/
void demo_factoryRestState_thread( void )
{
    while(1)
    {
        if(demo_getFactoryRestState() == RGBMIN_DO_FACTORYREST )
        {
            
            demo_doFactoryRest();
            demo_rest();
         }
        demo_delay_milliseconds(500);
    }
    return;
}
