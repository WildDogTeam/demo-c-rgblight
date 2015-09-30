/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_rgbLed.c
 *
 * Description: binServer API .
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

#include "demo_api.h"

#ifdef WILDDOG_PORT_TYPE_WICED
#include "wiced.h"

/*
WICED_PWM_3<--->GPIO9 <-->PA1<--->R
WICED_PWM_8<--->GPIO36<-->PA3<--->G
WICED_PWM_7<--->GPIO37<-->PA0<--->B
WICED_PWM_5<--->GPIO8<-->PA2<---->L
WICED_GPIO_5 <--->PA15<--->KEY

Press key >5s  factory reset
*/
#define LED_RED_PIN        WICED_PWM_3//8
#define LED_GREEN_PIN      WICED_PWM_8//WICED_PWM_7
#define LED_BLUE_PIN       WICED_PWM_7//WICED_PWM_8

#define LED_INTERNSITY_PIN       WICED_PWM_5

static Led_Color_T l_led_color;
static Blink_Type_T l_blink_type;
#else
#include <sys/socket.h>
#include <netinet/in.h>

#endif
/*todo */
/* should be in demo_rgbled.c */
#define RGB_LED_NUM 4
typedef struct RGBLED_LED_T
{
    u8* str;
    u8  status;
}RgbLed_Led_T;
typedef enum{
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_INTENSITY
}LED_COLOR;

static RgbLed_Led_T led_data[RGB_LED_NUM];
static char ledswitch[] = "sw";
static u8 l_rgbtree[][16]=
	{
		 "red",
		 "green",
		 "blue",
		 "intensity",
	};
void demo_deviceInit(void)
{
    int i = 0;
    memset(led_data, 0, sizeof(RgbLed_Led_T) * RGB_LED_NUM);
    for(i = 0; i < RGB_LED_NUM; i++)
    {
        led_data[i].str = l_rgbtree[i];
        led_data[i].status = FALSE;
    }

#ifdef WILDDOG_PORT_TYPE_WICED
    wiced_pwm_init ( LED_RED_PIN, LED_PWM_FREQ_HZ, 0 );   //red
    wiced_pwm_init ( LED_GREEN_PIN, LED_PWM_FREQ_HZ, 0 );  //green
    wiced_pwm_init ( LED_BLUE_PIN, LED_PWM_FREQ_HZ, 0 );  //blue
    wiced_pwm_init ( LED_INTERNSITY_PIN, LED_PWM_FREQ_HZ, 0 );  //intensity
#endif

}
void demo_deviceSet(int r, int g, int b,int iv)
{
    
    //printf("r:%d  g:%d  b:%d iv:%d \n", r, g, b,iv);
    
#ifdef WILDDOG_PORT_TYPE_WICED
    wiced_pwm_init ( LED_RED_PIN, LED_PWM_FREQ_HZ, r*100/255 );   //red
    wiced_pwm_start( LED_RED_PIN );

    wiced_pwm_init ( LED_GREEN_PIN, LED_PWM_FREQ_HZ, g*100/255 );  //green
    wiced_pwm_start( LED_GREEN_PIN );

    wiced_pwm_init ( LED_BLUE_PIN, LED_PWM_FREQ_HZ, b*100/255 );  //blue
    wiced_pwm_start( LED_BLUE_PIN );

#endif
}
STATIC void demo_deviceSetColor(Led_Color_T color)
{
    if(color == LED_COLOR_NONE)
        demo_deviceSet(0,0,0,0);
    else if(color == LED_COLOR_RED)
        demo_deviceSet(255,0,0,255);
    else if(color == LED_COLOR_ORANGE)
        demo_deviceSet(255,97,0,255);
    else if(color == LED_COLOR_YELLOW)
        demo_deviceSet(255,255,0,255);
    else if(color == LED_COLOR_GREEN)
        demo_deviceSet(0,255,0,255);
    else if(color == LED_COLOR_CYAN)
        demo_deviceSet(0,255,0,255);
    else if(color == LED_COLOR_BLUE)
        demo_deviceSet(0,0,255,255);
    else if(color == LED_COLOR_PURPLE)
        demo_deviceSet(160,32,240,255);
    else if(color == LED_COLOR_WHITE)
        demo_deviceSet(255,255,255,255);
}
void rgbLed_setLedstatus(Led_Color_T color, Blink_Type_T type)
{
    l_led_color = color;
    l_blink_type = type;
}

STATIC void rgbled_changeColoredLight(void)
{
    switch(l_blink_type)
    {
        case BLINK_SOLID:
            demo_deviceSetColor(l_led_color);
            break;
        case BLINK_QUICK:
          
            demo_deviceSetColor(l_led_color);
            demo_delay_milliseconds(200);
            demo_deviceSet(0,0,0,0);
            demo_delay_milliseconds(200);
            break;
        case BLINK_SLOW:

            demo_deviceSetColor(l_led_color);
            demo_delay_milliseconds(1000);
            demo_deviceSet(0,0,0,0);
            demo_delay_milliseconds(1000);
            break;
    }
    
    return;
}
void  demo_device_thread(void)
{

    while(1)
    {
        rgbled_changeColoredLight();
        demo_delay_milliseconds(20);
    }
    return;
}
Wilddog_Node_T *demo_buildDevicesTree(void)
{

    Wilddog_Node_T *p_head = NULL,*p_node = NULL;
    Wilddog_Node_T *p_red = NULL, *p_green = NULL;
    Wilddog_Node_T *p_bule = NULL,*p_intensity = NULL,*p_sw = NULL;
    
    /* creat device tree */
    p_head = wilddog_node_createObject((Wilddog_Str_T*)"head");
    p_node = wilddog_node_createObject((Wilddog_Str_T*)"led");
    p_red = wilddog_node_createUString((Wilddog_Str_T*)"red",\
        (Wilddog_Str_T*)"255");
    p_green = wilddog_node_createUString((Wilddog_Str_T*)"green",\
        (Wilddog_Str_T*)"255");
    p_bule = wilddog_node_createUString((Wilddog_Str_T*)"blue",\
        (Wilddog_Str_T*)"255");
    p_sw = wilddog_node_createUString((Wilddog_Str_T*)"sw",\
        (Wilddog_Str_T*)"1");
    p_intensity = wilddog_node_createUString((Wilddog_Str_T*)"intensity",\
        (Wilddog_Str_T*)"255");

    wilddog_node_addChild(p_head, p_node);
    wilddog_node_addChild(p_node, p_red);
    wilddog_node_addChild(p_node, p_green);
    wilddog_node_addChild(p_node, p_bule);
    wilddog_node_addChild(p_node, p_sw);
    wilddog_node_addChild(p_node, p_intensity);
    printf("devices's tree :\t");
    wilddog_debug_printnode(p_head);
    printf("\n");
    
    return p_head;
}
void demo_parseDeviceTree(const Wilddog_Node_T* p_snapshot)
{
    int i, flag = TRUE;
    Wilddog_Node_T * head = NULL, *curr = NULL, *tmp = NULL;
    int r = 0, g = 0, b = 0,iv =0;
    int led_switch = 0;

#if 0
    wilddog_debug_printnode(p_snapshot);
    printf("\n");
#endif    
    if(p_snapshot && p_snapshot->p_wn_child)
        head = p_snapshot->p_wn_child;
    if(head && head->p_wn_child)
        head = head->p_wn_child;
    if(p_snapshot->d_wn_type == WILDDOG_NODE_TYPE_NULL)
    {
        printf("null!!!!!!!!!!!\n");
        return;
    }
    for(i = 0; i < RGB_LED_NUM; i++)
    {
        flag = 1;
        
        for(curr = head; curr && (tmp = curr->p_wn_next, flag); curr = tmp)
        {
            if(curr->p_wn_key)
            {
            
                    if(0 == strncmp((const char *)(curr->p_wn_key), \
                        (const char *)(led_data[i].str), (strlen((const char *)(led_data[i].str))) ))
                    {
                    
                          if(i == LED_RED)
                          {
                              r = atoi((const char *)curr->p_wn_value);
                          }
                          if(i == LED_GREEN)
                          {
                              g = atoi((const char *)curr->p_wn_value);
                          }
                          if(i == LED_BLUE)
                          {
                              b = atoi((const char *)curr->p_wn_value);
                          }
						  if(i == LED_INTENSITY)
	                      {
	                          iv = atoi((const char *)curr->p_wn_value);
	                      }
                    }
            }
        }
    }
    
    for(curr = head; curr && (tmp = curr->p_wn_next, flag); curr = tmp)
	{
	    if(curr->p_wn_key)
	    {
	        //printf("key:%s\n", curr->p_wn_key);
	        if(0 == strncmp((const char *)(curr->p_wn_key), ledswitch, strlen(ledswitch)))
	        {
	            led_switch = atoi((const char *)curr->p_wn_value);
				
				//printf("r:%d  g:%d  b:%d iv:%d \n", r, g, b,iv);
                if(led_switch == 1)
			        demo_deviceSet( r, g, b,iv);
			    else
			        demo_deviceSet(0,0,0,0);
	        }
	    }
	}

    return;
}
#ifdef WILDDOG_PORT_TYPE_WICED

#endif
