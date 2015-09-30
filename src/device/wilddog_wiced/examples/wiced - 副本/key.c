
#include "platform.h"
#include "platform_peripheral.h"
#include "rtos.h"
#include "key.h"
#include "wiced.h"


char button_flag;
static wiced_time_t l_keyFallingTm = 0;

#if 0
void smartconfig_key_irqhandle(void)
{
    wiced_gpio_output_high(WICED_GPIO_34);
    wiced_gpio_output_high(WICED_GPIO_35);

    button_flag = SMART_CONFIG_FLAG;
    return;
}


void factoryreset_key_irqhandle(void)
{
    wiced_gpio_output_high(WICED_GPIO_36);
    wiced_gpio_output_high(WICED_GPIO_37);

    button_flag = FACTORY_RESET_FLAG;
    return;
}
void key_fallingFunc(void)
{
	wiced_time_get_time	( &l_keyFallingTm ); 
	printf("key_fallingFunc time =%d \n",l_keyFallingTm);
}

//wiced_bool_t wiced_gpio_input_get  ( wiced_gpio_t  gpio ) ;
void key_init(void)
{
#if 0
    wiced_gpio_init(WICED_GPIO_19,INPUT_PULL_UP );
    wiced_gpio_input_irq_enable(WICED_GPIO_19, IRQ_TRIGGER_BOTH_EDGES, smartconfig_key_irqhandle, NULL);

    wiced_gpio_init(WICED_GPIO_18,INPUT_PULL_UP );
    wiced_gpio_input_irq_enable(WICED_GPIO_18, IRQ_TRIGGER_BOTH_EDGES, factoryreset_key_irqhandle, NULL);
#else
	wiced_gpio_init(WICED_GPIO_5,INPUT_PULL_UP );

  //  wiced_gpio_input_irq_enable(DEMO_KEY_GPIO, IRQ_TRIGGER_BOTH_EDGES, key_risingFunc, NULL);


#endif
    return;
}
#endif





