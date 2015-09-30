/*
 * Copyright 2015, Wilddog Corporation
 * All Rights Reserved.
 */

/** @file
 *  RGB LED
 *  @ Created on : 2015/8/8
 *  @     Author : Baikal.Hu
 */

#include <stdlib.h>
#include "wiced.h"
#include "wilddog.h"
#include "wilddog_debug.h"

#include "rgbled.h"

int led_status_changed;

/*
 *           R
 *  PA1 ---/\/\/\---- R-LED ----+
 *           R                  |
 *  PA2 ---/\/\/\---- G-LED ----- GND
 *           R                  |
 *  PA6 ---/\/\/\---- B-LED ----+
 */
/******************************************************
 *                      Macros
 ******************************************************/
#define LED_PWM_FREQ_HZ 2000

/* for 747 */
#if 0
#define LED_GREEN      WICED_PWM_4     //   PA1
#define LED_RED        WICED_PWM_3     //   PA2
#define LED_BLUE       WICED_PWM_5//6     //   PA6
#else
/*
WICED_PWM_3<--->GPIO9 <-->PA1<--->R
WICED_PWM_8<--->GPIO36<-->PA3<--->G
WICED_PWM_7<--->GPIO37<-->PA0<--->B

WICED_PWM_5<--->GPIO8<-->PA2<---->L

WICED_GPIO_5 <--->PA15<--->KEY

Press key <5s  smnt
Press key >5s  factory reset
*/
#define LED_RED_PIN        WICED_PWM_3//8
#define LED_GREEN_PIN      WICED_PWM_8//WICED_PWM_7
#define LED_BLUE_PIN       WICED_PWM_7//WICED_PWM_8

#define LED_INTERNSITY_PIN       WICED_PWM_5


#endif

/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
/******************************************************
 *               Variable Definitions
 ******************************************************/

static Led_Color_T led_color;
static Blink_Type_T blink_type;

/******************************************************
 *               Function Definitions
 ******************************************************/

void rgb_led_init(void)
{
    wiced_pwm_init ( LED_RED_PIN, LED_PWM_FREQ_HZ, 0 );   //red
    wiced_pwm_init ( LED_GREEN_PIN, LED_PWM_FREQ_HZ, 0 );  //green
    wiced_pwm_init ( LED_BLUE_PIN, LED_PWM_FREQ_HZ, 0 );  //blue
	wiced_pwm_init ( LED_INTERNSITY_PIN, LED_PWM_FREQ_HZ, 0 );  //intensity

}
void set_rgb_led(int r, int g, int b,int iv)
{
	
/*
    wiced_pwm_stop( LED_RED_PIN );
    wiced_pwm_stop( LED_GREEN_PIN );
    wiced_pwm_stop( LED_BLUE_PIN );
    wiced_pwm_stop( LED_INTERNSITY_PIN );
*/


    wiced_pwm_init ( LED_RED_PIN, LED_PWM_FREQ_HZ, r*100/255 );   //red
    wiced_pwm_start( LED_RED_PIN );
	
    wiced_pwm_init ( LED_GREEN_PIN, LED_PWM_FREQ_HZ, g*100/255 );  //green
    wiced_pwm_start( LED_GREEN_PIN );

 	wiced_pwm_init ( LED_BLUE_PIN, LED_PWM_FREQ_HZ, b*100/255 );  //blue
    wiced_pwm_start( LED_BLUE_PIN );
#if 0
 	wiced_pwm_init ( LED_INTERNSITY_PIN, LED_PWM_FREQ_HZ, iv*100/255 );  //intensity
  	wiced_pwm_start( LED_INTERNSITY_PIN );
#endif	
}


void set_led_status(Led_Color_T color, Blink_Type_T type)
{
    led_color = color;
    blink_type = type;
}

void set_color(Led_Color_T color)
{
    if(color == LED_COLOR_NONE)
        set_rgb_led(0,0,0,0);
    else if(color == LED_COLOR_RED)
        set_rgb_led(255,0,0,255);
    else if(color == LED_COLOR_ORANGE)
        set_rgb_led(255,97,0,255);
    else if(color == LED_COLOR_YELLOW)
        set_rgb_led(255,255,0,255);
    else if(color == LED_COLOR_GREEN)
        set_rgb_led(0,255,0,255);
    else if(color == LED_COLOR_CYAN)
        set_rgb_led(0,255,0,255);
    else if(color == LED_COLOR_BLUE)
        set_rgb_led(0,0,255,255);
    else if(color == LED_COLOR_PURPLE)
        set_rgb_led(160,32,240,255);
    else if(color == LED_COLOR_WHITE)
        set_rgb_led(255,255,255,255);
}

void led_status()
{
    switch(blink_type)
    {
        case BLINK_SOLID:
        {
            set_color(led_color);
            return;
        }

        case BLINK_QUICK:
        {
            set_color(led_color);
            wiced_rtos_delay_milliseconds(200);
            set_rgb_led(0,0,0,0);
            wiced_rtos_delay_milliseconds(200);
            return;
        }

        case BLINK_SLOW:
        {
            set_color(led_color);
            wiced_rtos_delay_milliseconds(1000);
            set_rgb_led(0,0,0,0);
            wiced_rtos_delay_milliseconds(1000);
            return;
        }
    }

    return;
}








