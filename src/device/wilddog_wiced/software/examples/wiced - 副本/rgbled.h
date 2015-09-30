#ifndef _RGBLED_H
#define _RGBLED_H


#ifdef __cplusplus
extern "C" {
#endif

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


void set_led_status(Led_Color_T color, Blink_Type_T type);
void rgb_led_init(void);
void led_status();
void set_rgb_led(int r, int g, int b,int iv);


#ifdef __cplusplus
} /*extern "C" */
#endif


#endif
