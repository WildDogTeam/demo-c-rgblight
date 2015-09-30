#include <stdio.h>
#include <stdlib.h>

#include "wilddog.h"
#include "wilddog_debug.h"
#include "wilddog_api.h"
#include "wiced.h"
#include "rgbled.h"

#define LED_NUM 4


typedef struct WILDDOG_LED_T
{
    u8* str;
    u8  status;
}Wilddog_Led_T;

static u8 l_string[][16]=
{
 "red",
 "green",
 "blue",
 "intensity",
};

typedef enum{
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_INTENSITY
}LED_COLOR;


static char ledswitch[] = "sw";

typedef enum{
    LED_OBSERVER,
    LED_QUERY
}LED_GET;

#define LED_MAXPIN  5
#define LED_REOBSERVE 1
static u8 checkObseverfig =0;
static u8 reobserve =0;
static u8 led_state[5];
static Wilddog_Led_T led_data[LED_NUM];


void led_querystate(void)
{
     checkObseverfig = LED_QUERY;
}
#define TESTLED_REST_INV    (20*1000)
#define TESTLED_DIFF(a,b)   ((a>b)?(a-b):(b-a))
static int led_requery(void)
{
    int res =0;
    static u32 oldtime =0;
    wiced_time_t currentTm_ms;
    wiced_time_get_time(&currentTm_ms);
    if(TESTLED_DIFF(oldtime,(u32)currentTm_ms)>= TESTLED_REST_INV)
        {
            res =1;
            printf("requery\n");
            oldtime = (u32)currentTm_ms;
            led_querystate();
        }
    return res;
}

void led_check(const u8 pin,const u8 state)
{
    if( checkObseverfig == LED_QUERY &&led_state[pin] != state)
    {
        reobserve = LED_REOBSERVE;
        checkObseverfig = LED_OBSERVER;
    }
}
STATIC  int led_reObserver(void)
{
    int res = reobserve;
    reobserve = 0;
    return res;
}
void led_getstate(const u8 pin,const u8 state)
{
    if(pin > LED_MAXPIN )
        return;
    led_check(pin,state);
}


int led_data_init()
{
    int i;
    memset(led_data, 0, sizeof(Wilddog_Led_T) * LED_NUM);
    for(i = 0; i < LED_NUM; i++)
    {
        led_data[i].str = l_string[i];
        led_data[i].status = FALSE;
    }
    return 0;
}

STATIC void test_onQueryFunc
    (
    const Wilddog_Node_T* p_snapshot,
    void* arg,
    Wilddog_Return_T err
    )
{
    int i, flag = TRUE;
    Wilddog_Node_T * head = NULL, *curr = NULL, *tmp = NULL;
    int r = 0, g = 0, b = 0,iv = 0;
    int led_switch = 0;

    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("query failed!, err = %d", err);
        reobserve = LED_REOBSERVE;
        return;
    }
    wilddog_debug("query data!");
    //wilddog_debug_printnode(p_snapshot);
    //printf("\n");
    if(p_snapshot && p_snapshot->p_wn_child)
        head = p_snapshot->p_wn_child;
    if(head && head->p_wn_child)
        head = head->p_wn_child;
    if(p_snapshot->d_wn_type == WILDDOG_NODE_TYPE_NULL)
    {
        printf("null!!!!!!!!!!!\n");
        return;
    }


    for(curr = head; curr && (tmp = curr->p_wn_next, flag); curr = tmp)
    {
        if(curr->p_wn_key)
        {
            if(0 == strncmp((const char *)(curr->p_wn_key), ledswitch, strlen(ledswitch)))
            {
                led_switch = atoi((const char *)curr->p_wn_value);
            }
        }
    }

    for(i = 0; i < LED_NUM; i++)
    {
        flag = 1;
        for(curr = head; curr && (tmp = curr->p_wn_next, flag); curr = tmp)
        {
            if(curr->p_wn_key)
            {
                if(0 == strncmp((const char *)(curr->p_wn_key), (const char *)(led_data[i].str), strlen((const char *)(led_data[i].str)) ))
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

    printf("r:%d  g:%d  b:%d iv: %d\n", r, g, b,iv);
    if(led_switch == 1)
        set_rgb_led( r, g, b,iv);
    else
        set_rgb_led(0,0,0,0);

    return;
}
STATIC void test_onObserveFunc
    (
    const Wilddog_Node_T* p_snapshot,
    void* arg,
    Wilddog_Return_T err
    )
{
    int i, flag = TRUE;
    Wilddog_Node_T * head = NULL, *curr = NULL, *tmp = NULL;
    int r = 0, g = 0, b = 0,iv =0;
    int led_switch = 0;

    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("observe failed!, err = %d", err);
        reobserve = LED_REOBSERVE;
        return;
    }
    wilddog_debug("observe data!");
    wilddog_debug_printnode(p_snapshot);
    printf("\n");
    if(p_snapshot && p_snapshot->p_wn_child)
        head = p_snapshot->p_wn_child;
    if(head && head->p_wn_child)
        head = head->p_wn_child;
    if(p_snapshot->d_wn_type == WILDDOG_NODE_TYPE_NULL)
    {
        printf("null!!!!!!!!!!!\n");
        return;
    }



    for(i = 0; i < LED_NUM; i++)
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
	        printf("key:%s\n", curr->p_wn_key);
	        if(0 == strncmp((const char *)(curr->p_wn_key), ledswitch, strlen(ledswitch)))
	        {
	            led_switch = atoi((const char *)curr->p_wn_value);
				
				printf("r:%d  g:%d  b:%d iv:%d \n", r, g, b,iv);
			    if(led_switch == 1)
			        set_rgb_led( r, g, b,iv);
			    else
			        set_rgb_led(0,0,0,0);
	        }
	    }
	}

    return;
}

#define TEST_URL_HEAD       "coap://"
#define TEST_URL_END    ".wilddogio.com/"

int test_led( char *url, int *linkdown)
{
    BOOL isFinished = FALSE;
    Wilddog_T wilddog;
    int count  = 0;
    int sendnum = 0;
    checkObseverfig =0;
    reobserve =0;
    led_data_init();

    wilddog = wilddog_initWithUrl((Wilddog_Str_T *)url);
    if(0 == wilddog)
    {
        wilddog_debug("new wilddog failed!");
        return 0;
    }
    //wilddog_debug_printUrl(wilddog);
    wilddog_addObserver(wilddog, WD_ET_VALUECHANGE, test_onObserveFunc, (void*)&isFinished);
    while(1)
    {
        count++;

        if(*linkdown)
        {
            isFinished = FALSE;
            break;
        }
        if(count / 100 > 0)
        {
            count = 0;
            wilddog_debug("time increase! num = %d",sendnum++);
        }

        if(led_requery())
            wilddog_getValue(wilddog,test_onQueryFunc, (void*)&isFinished);

        if(led_reObserver())
        {
            wilddog_debug("reset!");
            wilddog_addObserver(wilddog, WD_ET_VALUECHANGE, test_onObserveFunc, (void*)&isFinished);
            //break;
        }

        wilddog_trySync();

    }

    wilddog_destroy(&wilddog);
    return 0;
}
