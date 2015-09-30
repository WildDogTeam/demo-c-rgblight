/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: demo_bindServer.c
 *
 * Description: bind server's API.
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
#include "wiced.h"
#include "wiced_tcpip.h"
#include "wifi_config_dct.h"
#include "wd_sdk_dct.h"
#include "wilddog_demo_config.h"
#include "demo_api.h"
#ifdef WILDDOG_PORT_TYPE_WICED
static wiced_ip_address_t l_src_ip_addr;
static uint16_t l_src_port;
static wiced_timed_event_t   wiced_binServer_recvEvent;


void demo_delay_milliseconds(u32 ms)
{
    wiced_rtos_delay_milliseconds(ms);

}
u32 demo_getSysTime_ms(void)
{
    wiced_time_t time;
    wiced_time_get_time(&time);
    return (u32)time;

}

u32 demo_getSysTime_sec(void)
{

    u32 current_time = demo_getSysTime_ms();
    if( current_time > SECONDS)
        return (u32)((u32)current_time/SECONDS);
    else 
        return 0;
}
/*
 * Function:    demo_creatUdpServer.
 * Description:build udp server that recveive broadcast packets that send by App.
 * Input:      
 * Output:     socketfd .
 * Return:   while  <0 mean build fault .
*/
int demo_creatUdpServer(int *fd)
{
    wiced_udp_socket_t* socket = NULL;

    socket = wmalloc( sizeof(wiced_udp_socket_t) );
    if ( NULL == socket )
     {
         printf( "%s malloc error!\n", __func__ );
         return -1;
     }

    if ( wiced_udp_create_socket( socket, BINDSERV_PORT, WICED_STA_INTERFACE )!= WICED_SUCCESS )
    {
        printf( "UDP socket creation failed\n");
        return -1;
    }
    *fd = (int)socket;
    /* Register a function to process received UDP packets */
    wiced_rtos_register_timed_event( &wiced_binServer_recvEvent, \
        WICED_NETWORKING_WORKER_THREAD, &bindServer_parseRecvPacket, 1*SECONDS, (void*)socket );
    printf("Waiting for UDP packets ...\n");
         
     return 0;

     
}
/*
 * Function:    demo_destoryUDPServer .
 * Description:  destory bindServer .
 * Input:    fd : bind server's socket .
 * Output:  N
 * Return:  N 
*/
void demo_destoryUDPServer(int fd)
{
    
    wilddog_closeSocket(fd);
    wiced_rtos_deregister_timed_event(&wiced_binServer_recvEvent);
    
}

int demo_sendUDPPacket 
	(
	int fd,
	char* buffer, 
	uint16_t buffer_length
	)
{
    wiced_packet_t*          packet;
    char*                    tx_data;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr,\
		GET_IPV4_ADDRESS(l_src_ip_addr) );

    wiced_udp_socket_t *pwiced_socket = (wiced_udp_socket_t*)fd;
    
    /* Create the UDP packet. Memory for the TX data is automatically allocated */
    if ( wiced_packet_create_udp( pwiced_socket, buffer_length, &packet, \
		(uint8_t**) &tx_data, &available_data_length ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP tx packet creation failed\n") );
        return -1;
    }

    /* Copy buffer into tx_data which is located inside the UDP packet */
    memcpy( tx_data, buffer, buffer_length + 1 );

    /* Set the end of the data portion of the packet */
    wiced_packet_set_data_end( packet, (uint8_t*) tx_data + buffer_length );

    /* Send the UDP packet */
    if ( wiced_udp_send( pwiced_socket, &target_ip_addr, l_src_port, packet ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP packet send failed\n") );
        /* Delete packet, since the send failed */
        wiced_packet_delete( packet );
        return -1;
    }
    else
    {
        WPRINT_APP_INFO( ("UDP Tx: %s\n\n", tx_data) );
    }
    
    return buffer_length;
}

int demo_recv
    (
    int fd,
    char *pbuf,
    u32 *pbuflen
    )
{
    wiced_udp_socket_t* socket = (wiced_udp_socket_t*)fd;

    wiced_packet_t*           packet;
    char*                     rx_data;
    static uint16_t           rx_data_length;
    uint16_t                  available_data_length;

    /* Wait for UDP packet */
    wiced_result_t result = wiced_udp_receive( socket, &packet, SECONDS );

    if ( ( result == WICED_ERROR ) || ( result == WICED_TIMEOUT ) )
    {
        return -1;
    }

    /* Get info about the received UDP packet */
    wiced_udp_packet_get_info( packet, &l_src_ip_addr, &l_src_port );

    /* Extract the received data from the UDP packet */
    wiced_packet_get_data( packet, 0, (uint8_t**) &rx_data, &rx_data_length, &available_data_length );

    if( *pbuflen > rx_data_length)
    {

        memcpy(pbuf,rx_data,rx_data_length);
        *pbuflen = rx_data_length;
        WPRINT_APP_INFO ( ("UDP Rx: \"%s\" from IP %u.%u.%u.%u:%d\n", rx_data,
    	  (unsigned char) ( ( GET_IPV4_ADDRESS(l_src_ip_addr) >> 24 ) & 0xff ),
    	  (unsigned char) ( ( GET_IPV4_ADDRESS(l_src_ip_addr) >> 16 ) & 0xff ),
    	  (unsigned char) ( ( GET_IPV4_ADDRESS(l_src_ip_addr) >>  8 ) & 0xff ),
    	  (unsigned char) ( ( GET_IPV4_ADDRESS(l_src_ip_addr) >>  0 ) & 0xff ),
    	  l_src_port ) );
    }
    else
        *pbuflen = 0;
    
    wiced_packet_delete( packet );
    
    return *pbuflen;
}
void demo_rest(void)
{
    
    NVIC_SystemReset();

}

/*
 * Function:    demo_getPinLeve .
 * Description:  read current pin's leve base on your platform .
 * Input:   demo_getPinLeve : pin number .
 * Output: N.      
 * Return:  N . 
*/
u8 demo_getPinLeve(u32 pin)
{
    return wiced_gpio_input_get(pin);
}

#else /* linux posix.*/

struct sockaddr clientAddr;
STATIC socklen_t l_clientaddrlen;
STATIC sockaddr l_clientAddr;

u32 demo_getSysTime_sec(void)
{
	struct timeval temtm; 
	gettimeofday(&temtm,NULL); 
    return (u32)temtm.tv_sec;
}
void demo_delay_milliseconds(u32 ms)
{

}

/*
 * Function:    demo_creatUdpServer.
 * Description:build udp server that recveive broadcast packets that send by App.
 * Input:      
 * Output:     socketfd .
 * Return:   while  <0 mean build fault .
*/
int demo_creatUdpServer(int *fd)
{
    
    struct sockaddr_in servaddr;

    /* create a socket */
    *fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if( *fd <0 )
    return *fd;
    /* init servaddr */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(BINDSERV_PORT);

    /* bind address and port to socket */
    if(bind(*fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
     perror("bind error");
     return -1;
    }

    printf("creat udp server listen to broadcast packets that send by App ,\n");
    printf("port : %d \n",BINDSERV_PORT);

    return 0;
}
int demo_recv
    (
    int fd,
    char *pbuf,
    u32 *pbuflen
    )
{
    return recvfrom(fd, pbuf, *pbuflen, MSG_DONTWAIT, &l_clientAddr, &l_clientaddrlen);

}
int demo_sendUDPPacket 
	(
	int fd,
	char* buffer, 
	uint16_t buffer_length
	)
{
    return sendto(fd,psendbuf,strlen(psendbuf),0,&l_clientAddr,l_clientaddrlen);

}
/*
 * Function:    demo_getPinLeve .
 * Description:  read current pin's leve base on your platform .
 * Input:   demo_getPinLeve : pin number .
 * Output: N.      
 * Return:  N . 
*/
STATIC u8 demo_getPinLeve(u32 pin)
{
    return 0;
}
void demo_rest(void)
{
    
}

#endif

