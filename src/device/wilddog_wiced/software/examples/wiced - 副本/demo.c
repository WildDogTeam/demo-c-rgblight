/*
 * main.c
 *
 *  Created on: 2015-08-08 Baikal
 */

#include <string.h>
#include "wiced.h"
#include "wilddog.h"
#include "wiced_tcpip.h"
#include "wifi_config_dct.h"
#include "wd_sdk_dct.h"
#include "wilddog_demo_config.h"
#include "rgbled.h"
#include "key.h"
#include "JSON.h"

#define MONITOR_THREAD_STACK_SIZE       1*1024
#define MAIN_THREAD_STACK_SIZE          5*1024
#define LED_THREAD_STACK_SIZE           1*1024
#define RX_WAIT_TIMEOUT                 (1*SECONDS)
#define PORTNUM                         (53364)           /* UDP port */
#define TEST_URL_HEAD                   "coap://"
#define TEST_URL_END                    ".wilddogio.com/"
#define TEST_HOST_END                   ".wilddogio.com"

#define FACTORYRESET_TIME	(5000)
#define SMNT_TIME			(1000)
#define DEMO_KEY_GPIO		(WICED_GPIO_2)
#define DIFF(a,b)	(a>b)?(a-b):(b-a)


#define WICEDPIN_LOW	WICED_FALSE
#define WICEDPIN_HIGHT	WICED_TRUE

#define APP_GET_DEVID   "get"
#define APP_SET_USERID  "set"

#define DEMO_ACKSTR_BINED  "{\"cmd\": \"ack\",\"subcmd\": {\"token\": \"success\",\"userId\": \"success\"}}"

#define DEMO_DEVICE_ID  "skylli"
#define DEMO_DEVICE_NAME    "wilddog_led"

static wiced_thread_t                   monitor_thread;
static wiced_thread_t                   main_thread;
static wiced_thread_t                   LED_thread;
static wiced_mutex_t                    dct_mutex;
static wiced_semaphore_t                getDevId_complete;
static wiced_timed_event_t              process_udp_rx_event;
static wiced_udp_socket_t               udp_socket;

int g_wifi_down = 1;

extern char button_flag;
char l_bound_state = 0;
char timeout_flag;


static char user_id[32];
static char device_id[32];
static u8 l_key_pressed = FALSE;
static unsigned long    l_isBinded = 0;

static wiced_ip_address_t l_src_ip_addr;
static uint16_t           l_src_port;

char cmd[32];
char token[256];
char userId[32];
char devId[32];


extern int demo(char *url, int* isUnConnected);
extern wiced_result_t comm( wiced_json_object_t* json_object );
static wiced_result_t send_udp_response 
	(
	char* buffer, 
	uint16_t buffer_length, 
	wiced_ip_address_t ip_addr, 
	uint32_t port
	);
void wiced_destory_binServer(void);


typedef struct ISFINISH
{
    BOOL  isCalled;
    BOOL  isTimeout;
    BOOL  isOver;
}IsFinish_T;

typedef enum WICED_BOUND_STATE_T
{
   BOUND_NONE,
   BOUND_ING,
   BOUND_DONE
}Wiced_Bound_State_T;
typedef enum WICED_DCT_STATE_T
{
   DEMO_WIFI_CONNECTED,
   DEMO_WIFI_DISCONNECTED,
   DEMO_DCT_ERR,
}Wiced_Dct_State_T;

void link_callback_up(void)
{
    printf("link_callback_up \n");
    g_wifi_down = 0;
}
void link_callback_down(void)
{
    printf("link_callback_down \n");
    g_wifi_down = 1;
}

void  led_thread_main(uint32_t arg)
{
    while(1)
    {
        led_status();
        wiced_rtos_delay_milliseconds(20);
    }
    return;
}
void key_fallingFunc(void)
{
	wilddog_debug();
	l_key_pressed = TRUE;	
}
void key_pressTarg(wiced_gpio_t keyPin)
{
	
	static wiced_time_t l_keypress_time = 0;
	u32 diff_temp = 0;
	wiced_time_t curr_tm = 0;
	
	 
	wiced_time_get_time ( &curr_tm ); 

	if( FALSE == l_key_pressed)
		return ;
	
	if( l_keypress_time == 0 )
	{
		
		if( WICEDPIN_LOW == wiced_gpio_input_get(keyPin))
		{
			l_keypress_time = curr_tm; 
			//printf("curr =%d;press_tm = %d,diff = %d \n",curr_tm,l_keypress_time, \
			//		 DIFF(curr_tm,l_keypress_time));
		}
	}
	else
	{
		diff_temp =(u32) DIFF(curr_tm,l_keypress_time);
		if(  diff_temp >= FACTORYRESET_TIME)
		{
			printf("11>>>curr =%d;press_tm = %d,diff = %d \n",curr_tm,l_keypress_time, \
					 diff_temp);
			button_flag = FACTORY_RESET_FLAG;	
			l_key_pressed = FALSE;
			l_keypress_time = 0;
			printf(">>>>FACTORY_RESET_FLAG \n");
		}
		else
		if( WICEDPIN_HIGHT == wiced_gpio_input_get(keyPin) )
		{
			
			l_keypress_time = 0;
		}
	}
}

void  monitor_thread_main(uint32_t arg)
{
    wiced_config_ap_entry_t* dct_ap_entry;
    user_dct_data_t* dct;
    char device_id[32];
	u8 while_cnt = 0;

	
	
    while(button_flag == NULL_FLAG)
    {
        wiced_rtos_delay_milliseconds(10);
		if(while_cnt++>100)
		{
			
			while_cnt = 0;
		}
		key_pressTarg(DEMO_KEY_GPIO);
    };

    WPRINT_APP_INFO(("monitor clean and restart!\n"));

    if(button_flag == SMART_CONFIG_FLAG)
    {
        /***clear the stored ap list***/
        WPRINT_APP_INFO(("smart config\n\n"));
        //wiced_rtos_lock_mutex( &dct_mutex );
        wiced_dct_read_lock( (void**) &dct_ap_entry, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
        memset(dct_ap_entry, 0, sizeof(wiced_config_ap_entry_t));
        wiced_dct_write( (const void*) dct_ap_entry, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
        wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );
        WPRINT_APP_INFO(("smart config222\n\n"));

		//wiced_rtos_unlock_mutex( &dct_mutex );
    }

    if(button_flag == FACTORY_RESET_FLAG)
    {
        WPRINT_APP_INFO(("factory reset\n\n"));
       // wiced_rtos_lock_mutex( &dct_mutex );
        wiced_dct_read_lock( (void**) &dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
        memcpy(device_id, dct->device_id, 32);
        memset(dct, 0, sizeof(user_dct_data_t));
        memcpy(dct->device_id, device_id, 32);
        /* test to do */
		memcpy(dct->device_id,DEMO_DEVICE_NAME,sizeof(DEMO_DEVICE_NAME));
		memcpy(dct->user_id, DEMO_DEVICE_ID,sizeof(DEMO_DEVICE_ID));
        dct->isBinded = 0;
        wiced_dct_write( (const void*) dct, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
        wiced_dct_read_unlock( dct, WICED_TRUE );
		
        wiced_dct_read_lock( (void**) &dct_ap_entry, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
        memset(dct_ap_entry, 0, sizeof(wiced_config_ap_entry_t));
        wiced_dct_write( (const void*) dct_ap_entry, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
        wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );

		//wiced_rtos_unlock_mutex( &dct_mutex );
    }
	//wiced_network_down(WICED_STA_INTERFACE);
	//wiced_deinit();
    /****reboot***/
	
	WPRINT_APP_INFO(("NVIC_SystemReset \n\n"));
    NVIC_SystemReset();
	
}



int getDCT_WIFIState()
{
    wiced_config_ap_entry_t* dct_ap_entry;
    wiced_mac_t* dct_mac;
    wiced_result_t result;
    user_dct_data_t* dct;
    int len;

    wiced_rtos_lock_mutex( &dct_mutex );
    
    /* get user configure */
    wiced_dct_read_lock( (void**) &dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );

    WPRINT_APP_INFO(("device_id:%s\n", dct->device_id));
    WPRINT_APP_INFO(("user_id:%s\n", dct->user_id));
    WPRINT_APP_INFO(("isbind:%u\n", dct->isBinded));

    memcpy( user_id, dct->user_id, 32 );
    //memcpy( device_id, dct->device_id, 32);
    l_isBinded = dct->isBinded;
    wiced_dct_read_unlock( dct, WICED_TRUE );
    wiced_rtos_unlock_mutex( &dct_mutex );



    wiced_rtos_lock_mutex( &dct_mutex );
    /* get wifi configure */
    wiced_dct_read_lock( (void**) &dct_ap_entry,\
        WICED_TRUE, DCT_WIFI_CONFIG_SECTION,\
        offsetof(platform_dct_wifi_config_t, stored_ap_list),\
        sizeof(wiced_config_ap_entry_t) );
    WPRINT_APP_INFO(("len:%d\n",dct_ap_entry->details.SSID.length));
    WPRINT_APP_INFO(("val:%s\n",dct_ap_entry->details.SSID.value));
    
    len = dct_ap_entry->details.SSID.length;
    wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );
    wiced_rtos_unlock_mutex( &dct_mutex );
    
    wiced_dct_read_lock( (void**) &dct_mac,\
        WICED_TRUE, DCT_WIFI_CONFIG_SECTION,\
        offsetof(platform_dct_wifi_config_t, mac_address),\
        sizeof(wiced_mac_t) );
    sprintf(device_id,"%02x:%02x:%02x",dct_mac->octet[3], dct_mac->octet[4], dct_mac->octet[5]);
    
    WPRINT_APP_INFO(("device_id :%s\n",device_id));
    wiced_dct_read_unlock( dct_mac, WICED_TRUE );
    if(len == 0)
    {
        return DEMO_WIFI_DISCONNECTED;
    }

    /* Bring up the network interface */
    int up_tries = 3;

    while (up_tries--) {
        WPRINT_APP_INFO(("in up_tries = %d\n",up_tries));
        result = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
        if (result == WICED_SUCCESS) {
            return DEMO_WIFI_CONNECTED;
        } else {
            WPRINT_APP_INFO(("Network up failed, try again (%d left)  error code :%d\n", up_tries, result));
        }
    }

    return DEMO_WIFI_DISCONNECTED;
}



int start_smartconfig()
{

    wiced_result_t result;
    wiced_config_ap_entry_t* dct_ap_entry;
    WPRINT_APP_INFO(("smart\n"));

    result = 1;

    /* enable protocols */
    easy_setup_enable_cooee(); /* broadcom cooee */
    easy_setup_enable_neeze(); /* broadcom neeze */
    //easy_setup_enable_akiss(); /* wechat akiss */
    //easy_setup_enable_changhong(); /* changhong */

    /* set cooee key */
    //cooee_set_key("1111111111111111");

    /* set airkiss key */
    //akiss_set_key("1111111111111111");

    /* start easy setup */
    if (easy_setup_start() != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("easy setup failed.\r\n"));
    }
    else
    {
        WPRINT_APP_INFO(("easy setup done.\r\n"));
    }

    int up_tries = 3;

    while (up_tries--) {
        result = wiced_network_up( WICED_STA_INTERFACE,\
			WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
        if (result == WICED_SUCCESS) {
            break;
        } else {
            WPRINT_APP_INFO(("Network up failed, try again (%d left)\r\n", up_tries));
        }
    }
    if ( result != WICED_SUCCESS )
    {
        wiced_dct_read_lock( (void**) &dct_ap_entry, WICED_TRUE, \
			DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list),\
			sizeof(wiced_config_ap_entry_t) );
		
        if (dct_ap_entry->details.security == WICED_SECURITY_WEP_PSK ) // Now try shared instead of open authentication
        {
            dct_ap_entry->details.security = WICED_SECURITY_WEP_SHARED;
            wiced_dct_write( dct_ap_entry, DCT_WIFI_CONFIG_SECTION, \
				offsetof(platform_dct_wifi_config_t, stored_ap_list),\
				sizeof(wiced_config_ap_entry_t) );
            result = wiced_network_up( WICED_STA_INTERFACE,\
				WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
            if ( result != WICED_SUCCESS ) // Restore old value
            {
                //wiced_dct_read_lock( (void**) &dct_ap_entry, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
                dct_ap_entry->details.security = WICED_SECURITY_WEP_PSK;
                wiced_dct_write( dct_ap_entry, DCT_WIFI_CONFIG_SECTION, \
					offsetof(platform_dct_wifi_config_t, stored_ap_list),\
					sizeof(wiced_config_ap_entry_t) );
            }
        }
		
        wiced_dct_read_unlock( dct_ap_entry, WICED_TRUE );
    }

    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("Network up failed\n") );
    }
    else
    {
        WPRINT_APP_INFO( ("Network up success\n") );
    }

    return result;
}

wiced_result_t wiced_binServer_recvParse
    (
    wiced_ip_address_t *P_udp_src_ip_addr,
    uint16_t           *P_udp_src_port
    )
{
    wiced_packet_t*           packet;
    char*                     rx_data;
    static uint16_t           rx_data_length;
    uint16_t                  available_data_length;
    user_dct_data_t*          dct;

    /* Wait for UDP packet */
    wiced_result_t result = wiced_udp_receive( &udp_socket, &packet, RX_WAIT_TIMEOUT );

    //WPRINT_APP_INFO(("udp result:%d\n", result));

    if ( ( result == WICED_ERROR ) || ( result == WICED_TIMEOUT ) )
    {
        return result;
    }

    /* Get info about the received UDP packet */
    wiced_udp_packet_get_info( packet, P_udp_src_ip_addr, P_udp_src_port );

    /* Extract the received data from the UDP packet */
    wiced_packet_get_data( packet, 0, (uint8_t**) &rx_data, &rx_data_length, &available_data_length );

    /* Null terminate the received data, just in case the sender didn't do this */
    rx_data[ rx_data_length ] = '\x0';

    WPRINT_APP_INFO ( ("UDP Rx: \"%s\" from IP %u.%u.%u.%u:%d\n", rx_data,
	  (unsigned char) ( ( GET_IPV4_ADDRESS(*P_udp_src_ip_addr) >> 24 ) & 0xff ),
	  (unsigned char) ( ( GET_IPV4_ADDRESS(*P_udp_src_ip_addr) >> 16 ) & 0xff ),
	  (unsigned char) ( ( GET_IPV4_ADDRESS(*P_udp_src_ip_addr) >>  8 ) & 0xff ),
	  (unsigned char) ( ( GET_IPV4_ADDRESS(*P_udp_src_ip_addr) >>  0 ) & 0xff ),
	  &P_udp_src_port ) );

    memset(cmd, 0, 32);
    wiced_JSON_parser( rx_data, strlen(rx_data));
    wiced_packet_delete( packet );
    
    return WICED_SUCCESS;
}
void wiced_binServer_saveDct(void)
{
    user_dct_data_t*          dct;
    wiced_dct_read_lock( (void**) &dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
    memcpy( dct->user_id, userId, 32);
    dct->isBinded = TRUE;
    wiced_dct_write( (const void*) dct, DCT_APP_SECTION, 0, sizeof(user_dct_data_t) );
    wiced_dct_read_unlock( dct, WICED_TRUE );

}
void wiced_binServer_recv(void)
{

    

    memset(cmd, 0, 32);
    wiced_binServer_recvParse(&l_src_ip_addr,&l_src_port);
    

    if( l_bound_state == BOUND_NONE &&\
        0 == strncmp(cmd, APP_GET_DEVID, strlen(APP_GET_DEVID)) )
    {
        
        char  tx_data[128];
        memset(tx_data, 0, 128);
        sprintf(tx_data,"%s%s%s", "{\"cmd\": \"ack\",\"subcmd\": {\"devId\": \"",\
            device_id, "\"}}");        
        send_udp_response( tx_data, strlen(tx_data), l_src_ip_addr, l_src_port );
    }
    else 
    if( 0 == strncmp(cmd, APP_SET_USERID, strlen(APP_SET_USERID)) )
    {
        if( l_bound_state == BOUND_NONE )
        {
            l_bound_state = BOUND_ING;
            
         }
        else
        if(l_bound_state == BOUND_DONE)
        {
            wiced_boundServer_tokenResponse();
        }
        
    }
    else
        return ;
    /* Echo the received data to the sender */
}
void wiced_build_udpServer(void)
{
    if ( wiced_udp_create_socket( &udp_socket, PORTNUM, WICED_STA_INTERFACE ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP socket creation failed\n") );
    }

    /* Register a function to process received UDP packets */
    wiced_rtos_register_timed_event( &process_udp_rx_event, \
        WICED_NETWORKING_WORKER_THREAD, &wiced_binServer_recv, 1*SECONDS, 0 );
    WPRINT_APP_INFO(("Waiting for UDP packets ...\n"));
    
}
void wiced_destory_binServer(void)
{
    
    l_bound_state = BOUND_DONE;
    wiced_rtos_deregister_timed_event(&process_udp_rx_event);
    wiced_udp_delete_socket(&udp_socket);

}
void wiced_boundServer_tokenResponse(void)
{
    char  tx_data[128];

    memset(tx_data,0,128);
    memcpy(tx_data,DEMO_ACKSTR_BINED,strlen(DEMO_ACKSTR_BINED));
    send_udp_response( tx_data, strlen(tx_data), l_src_ip_addr, l_src_port );
    wiced_destory_binServer();
}
#if 0
void  wiced_boundServer_thread(uint32_t arg)
{
    /*1. build udp server listern app cmd and respond */
    wiced_build_udpServer();
    /*2.*/
    
    
    return;
}
#endif

static wiced_result_t send_udp_response 
	(
	char* buffer, 
	uint16_t buffer_length, 
	wiced_ip_address_t ip_addr, 
	uint32_t port
	)
{
    wiced_packet_t*          packet;
    char*                    tx_data;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr,\
		GET_IPV4_ADDRESS(ip_addr) );

    /* Create the UDP packet. Memory for the TX data is automatically allocated */
    if ( wiced_packet_create_udp( &udp_socket, buffer_length, &packet, \
		(uint8_t**) &tx_data, &available_data_length ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP tx packet creation failed\n") );
        return WICED_ERROR;
    }

    /* Copy buffer into tx_data which is located inside the UDP packet */
    memcpy( tx_data, buffer, buffer_length + 1 );

    /* Set the end of the data portion of the packet */
    wiced_packet_set_data_end( packet, (uint8_t*) tx_data + buffer_length );

    /* Send the UDP packet */
    if ( wiced_udp_send( &udp_socket, &target_ip_addr, port, packet ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP packet send failed\n") );
        /* Delete packet, since the send failed */
        wiced_packet_delete( packet );
    }
    else
    {
        WPRINT_APP_INFO( ("UDP Tx: %s\n\n", tx_data) );
    }

    /*
     * NOTE : It is not necessary to delete the packet created above, the packet
     *        will be automatically deleted *AFTER* it has been successfully sent
     */

    return WICED_SUCCESS;
}

void server_timer_handle(void *arg)
{
    wiced_rtos_stop_timer((wiced_timer_t *)arg);
    //wiced_rtos_deinit_timer((wiced_timer_t *)arg);
    //wiced_rtos_deregister_timed_event(&process_udp_rx_event);
    //wiced_udp_delete_socket(&udp_socket);
    timeout_flag = 1;
    //WPRINT_APP_INFO(("server time out\n"));
}

STATIC void test_addObserverFunc
    (
    const Wilddog_Node_T* p_snapshot,
    void* arg,
    Wilddog_Return_T err
    )
{
    printf("addobserve err:%d\n", err);
    ((IsFinish_T *)arg)->isCalled = TRUE;

    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        WPRINT_APP_INFO(("addObserver failed!"));
        ((IsFinish_T *)arg)->isOver = TRUE;
        return;
    }
    wilddog_debug_printnode(p_snapshot);
    printf("\n");
    printf("key:%s\n", p_snapshot->p_wn_key);


    if((p_snapshot->d_wn_type == WILDDOG_NODE_TYPE_OBJECT) && (p_snapshot->p_wn_child == NULL))
    {
        printf("isover1\n");
        ((IsFinish_T *)arg)->isOver = TRUE;
    }



    if(p_snapshot->p_wn_key == NULL && p_snapshot->d_wn_type == WILDDOG_NODE_TYPE_BYTESTRING)
    {
        printf("isover2\n");
        ((IsFinish_T *)arg)->isOver = TRUE;
    }




    if(err == WILDDOG_ERR_RECVTIMEOUT)
    {
        ((IsFinish_T *)arg)->isTimeout = TRUE;
    }

    WPRINT_APP_INFO(("addObserver data!"));

    return;
}

STATIC void test_authFunc(void* arg, Wilddog_Return_T err)
{

    ((IsFinish_T *)arg)->isCalled = TRUE;

    printf("err:%d\n", err);
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("auth error!");
        return;
    }
    wilddog_debug("auth success!");
    if(err == WILDDOG_ERR_RECVTIMEOUT)
    {
        ((IsFinish_T *)arg)->isTimeout = TRUE;
    }
    else
    {
        ((IsFinish_T *)arg)->isOver = TRUE;
    }
    return;
}

STATIC void test_setValueFunc(void* arg, Wilddog_Return_T err)
{
    ((IsFinish_T *)arg)->isCalled = TRUE;
    printf("setValue error code:%d\n", err);

    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("setValue error!");
        return;
    }
    wilddog_debug("setValue success!");
    if(err == WILDDOG_ERR_RECVTIMEOUT)
    {
        ((IsFinish_T *)arg)->isTimeout = TRUE;
    }
    else
    {
        ((IsFinish_T *)arg)->isOver = TRUE;
    }
    return;
}




void main_thread_main(uint32_t arg)
{
    char url[256];
    char host[256];
    int ret = 0;
    IsFinish_T isFinish;
    isFinish.isCalled = FALSE;
    isFinish.isTimeout = FALSE;
    isFinish.isOver = FALSE;

    wiced_timer_t server_timer;
    BOOL isBind = FALSE;
    Wilddog_T wilddog1 = 0;
    Wilddog_T wilddog2 = 0;

    WPRINT_APP_INFO(("main start\n\n"));

    memset(cmd, 0, strlen(cmd));
    memset(token, 0, strlen(token));
    memset(userId, 0, strlen(userId));
    memset(devId, 0, strlen(devId));

    do
    {
         /*
          *
          * 监听数据，如果数据不存在，或者userId不存在，则直接跳到未绑定
          *
          * */

         int trytimes = 4;
         int resul;
#if 1
         while(trytimes--)
         {
             memset(url, 0, 256);

             sprintf(url,"%szhinengcaideng%s%s/%s/%s",TEST_URL_HEAD,TEST_URL_END,"devicemanage",user_id,device_id);
             //sprintf(url,"%stest_led%s%s/%s/%s",TEST_URL_HEAD,TEST_URL_END,"devicemanage",user_id,device_id);

             WPRINT_APP_INFO(("url:%s\n", url));

             wilddog1 = wilddog_initWithUrl((Wilddog_Str_T *)url);
             if(0 == wilddog1)
             {
                 wilddog_debug("new wilddog failed!");
                 isBind = FALSE;
                 break;
             }

             resul = wilddog_addObserver(wilddog1, WD_ET_VALUECHANGE, \
			 	test_addObserverFunc, (void*)(&isFinish));

             WPRINT_APP_INFO(("resul:%d\n", resul));

             while(!isFinish.isCalled)
             {
                 WPRINT_APP_INFO(("..\n"));
                 wilddog_trySync();
             }
             WPRINT_APP_INFO(("\nafter try sync\n\n"));

             while(trytimes == 1)
             {
                 //  重试三次仍然超时，则慢闪 Green。
                 set_led_status(LED_COLOR_GREEN, BLINK_SLOW);
                 wiced_rtos_delay_milliseconds(1000);
             }

             if(isFinish.isTimeout)           //超时重试
             {
                 //reg_led_status(LED_COLOR_RED);
                 printf("time out\n\n");
                 continue;
             }
             else if(isFinish.isOver)     //返回的是空节点，证明数据不存在
             {
                 WPRINT_APP_INFO(("the data is not exist, then goto bind\n"));
                 isBind = FALSE;
                 break;
             }
             else
             {
                 WPRINT_APP_INFO(("the data is exist, it has binded\n"));
                 if( l_isBinded  )
                 {
                     isBind = TRUE;
                 }   
                 break;
             }
         }
#endif

         WPRINT_APP_INFO(("isBind:%d\n", isBind));
         wilddog_destroy(&wilddog1);
         set_led_status(LED_COLOR_BLUE, BLINK_QUICK);
         WPRINT_APP_INFO(("111111111\n\n"));
#if 1
         if( FALSE == isBind )
         {
             wiced_rtos_init_timer(&server_timer, 60*SECONDS, server_timer_handle, (void *)(&server_timer));
             wiced_rtos_start_timer(&server_timer);


             
             while(timeout_flag == 0)
             {

                 while( BOUND_NONE== l_bound_state )
                 {
                     wiced_rtos_delay_milliseconds(1000);

                     //WPRINT_APP_INFO(("did not receive the set cmd timeout_flag!:%d\n", timeout_flag));
                     if(1 == timeout_flag)
                     {
                         //WPRINT_APP_INFO(("break1\n"));
                         wiced_rtos_deinit_timer(&server_timer);
                         break;
                     }
                 }
                 //WPRINT_APP_INFO(("aaa\n"));
                 if(1 == timeout_flag)
                     break;
                 WPRINT_APP_INFO(("bbb\n"));

                 memset(url, 0, 256);
                 sprintf(url,"%szhinengcaideng%s%s/%s/%s",TEST_URL_HEAD,TEST_URL_END,"devicemanage",userId,device_id);
                 //sprintf(url,"%stest_led%s%s/%s/%s",TEST_URL_HEAD,TEST_URL_END,"devicemanage",userId,device_id);

                 WPRINT_APP_INFO(("bind userId: %s url:%s\n", userId, url));

                 wilddog2 = wilddog_initWithUrl((Wilddog_Str_T *)url);
                 if(0 == wilddog2)
                 {
                     wilddog_debug("new wilddog failed!");
                     return;
                 }


                 Wilddog_Node_T *p_head,*p_node;
                 Wilddog_Node_T *p_red, *p_green, *p_bule,*p_intensity,*p_sw;

                 p_head = wilddog_node_createObject((Wilddog_Str_T*)"head");
                 p_node = wilddog_node_createObject((Wilddog_Str_T*)"led");
                 p_red = wilddog_node_createUString((Wilddog_Str_T*)"red", (Wilddog_Str_T*)"255");
                 p_green = wilddog_node_createUString((Wilddog_Str_T*)"green", (Wilddog_Str_T*)"255");
                 p_bule = wilddog_node_createUString((Wilddog_Str_T*)"blue", (Wilddog_Str_T*)"255");
                 p_sw = wilddog_node_createUString((Wilddog_Str_T*)"sw", (Wilddog_Str_T*)"1");
				 p_intensity = wilddog_node_createUString((Wilddog_Str_T*)"intensity", (Wilddog_Str_T*)"255");

                 wilddog_node_addChild(p_head, p_node);
                 wilddog_node_addChild(p_node, p_red);
                 wilddog_node_addChild(p_node, p_green);
                 wilddog_node_addChild(p_node, p_bule);
                 wilddog_node_addChild(p_node, p_sw);
				 wilddog_node_addChild(p_node, p_intensity);

                 /**First send auth, then put the tree**/
                 memset(host, 0, 256);
                 sprintf(host,"zhinengcaideng%s",TEST_HOST_END);
                 //sprintf(host,"test_led%s",TEST_HOST_END);
#if 1
                 isFinish.isCalled = FALSE;
                 isFinish.isTimeout = FALSE;
                 isFinish.isOver = FALSE;

#if 1
                 WPRINT_APP_INFO(("send auth: %s\n\n", token));
                 WPRINT_APP_INFO(("host:%s\n\n", host));
                 wilddog_auth(host, token, strlen(token), test_authFunc, (void*)(&isFinish));
                 while(!isFinish.isCalled)
                 {
                     WPRINT_APP_INFO(("..\n"));
                     wilddog_trySync();
                 }
#endif

        isFinish.isCalled = FALSE;
        isFinish.isTimeout = FALSE;
        isFinish.isOver = FALSE;
        wilddog_setValue(wilddog2, p_head, test_setValueFunc, (void*)(&isFinish));
        //wilddog_addObserver(wilddog2, WD_ET_VALUECHANGE, test_addObserverFunc, (void*)(&isFinish));
        wilddog_node_delete(p_head);


                 while(!isFinish.isCalled)
                 {
                     WPRINT_APP_INFO(("..\n"));
                     wilddog_trySync();
                 }

                 if(isFinish.isTimeout)           //超时重试
                 {
                     continue;
                 }
                 else if(isFinish.isOver)
                 {
                     WPRINT_APP_INFO(("bind process success\n"));
                     l_bound_state = BOUND_DONE;
                     isBind = TRUE;
                     wiced_binServer_saveDct();
                     break;
                 }

#endif

             }

         }
#endif
         WPRINT_APP_INFO(("222222222\n\n"));
         //wiced_rtos_deregister_timed_event(&process_udp_rx_event);
         //wiced_udp_delete_socket(&udp_socket);

         WPRINT_APP_INFO(("333333333\n\n"));
         wilddog_destroy(&wilddog2);
         /**绑定超时，蓝灯慢闪**/
         if(1 == timeout_flag)
         {
             WPRINT_APP_INFO(("bind timeout\n"));
             set_led_status(LED_COLOR_BLUE, BLINK_SLOW);
             while(1)
             {
                 wiced_rtos_delay_milliseconds(1000);
             }
         }

         WPRINT_APP_INFO(("wtf  isBind:%d\n", isBind));
         //WPRINT_APP_INFO(("wtf2  isBind:%d\n", isBind));

         if(TRUE == isBind)
         {
             g_wifi_down = 0;
             printf("test the led\n\n");
             wiced_rtos_delete_thread(&LED_thread);
             test_led(url, &g_wifi_down);
         }
         else
         {
             WPRINT_APP_INFO(("bind failed\n"));
             set_led_status(LED_COLOR_BLUE, BLINK_SLOW);
             while(1)
             {
                 wiced_rtos_delay_milliseconds(1000);
             }
         }
 

		 wilddog_debug();
         wiced_network_deregister_link_callback(NULL, NULL);
         wiced_network_down(WICED_STA_INTERFACE);
         wiced_deinit();
    }while(0);
}


/**
 *  Application start
 */
void application_start( void )
{
    wiced_result_t result;

    /**device start red led blink one time*/
    set_led_status(LED_COLOR_RED, BLINK_SOLID);
    wiced_rtos_delay_milliseconds(100);

    /** Initialize the device **/
    button_flag = NULL_FLAG;
    l_bound_state = 0;

    timeout_flag = 0;

    wiced_init();

	wiced_gpio_init(DEMO_KEY_GPIO,INPUT_PULL_UP );
	wiced_gpio_input_irq_enable(DEMO_KEY_GPIO,\
		IRQ_TRIGGER_FALLING_EDGE, key_fallingFunc, NULL);
    wiced_rtos_init_semaphore( &getDevId_complete );
    wilddog_debug();

    /* Create a mutex for DCT access */
    wiced_rtos_init_mutex( &dct_mutex );


    wiced_rtos_create_thread(&LED_thread,               \
                              WICED_APPLICATION_PRIORITY,\
                              "LED",        \
                              &led_thread_main,           \
                              LED_THREAD_STACK_SIZE,(void*)NULL);

    wiced_rtos_create_thread(&monitor_thread,               \
                              WICED_DEFAULT_WORKER_PRIORITY,\
                              "Monitor",        \
                              &monitor_thread_main,           \
                              MONITOR_THREAD_STACK_SIZE,(void*)NULL );
    
   set_led_status(LED_COLOR_RED, BLINK_QUICK);
    /*
     * register the json callback function, it will parse the command from the app
     */
     wiced_JSON_parser_register_callback(comm);

    if( DEMO_WIFI_CONNECTED != getDCT_WIFIState())
    {
        result = start_smartconfig();
		
        if(WICED_SUCCESS != result)
        {
            set_led_status(LED_COLOR_RED, BLINK_SLOW);
            while(1)
            {
                 wiced_rtos_delay_milliseconds(1000);
            }
        }
        else
        {
            wiced_build_udpServer();
        }
    }
    else
    {
        WPRINT_APP_INFO( ("Network up success!\n") );
    }


    wiced_rtos_create_thread(&main_thread,               \
                              WICED_APPLICATION_PRIORITY,\
                              "Main",        \
                              &main_thread_main,           \
                              MAIN_THREAD_STACK_SIZE,(void*)NULL );
	
}


