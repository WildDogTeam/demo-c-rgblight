/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wiced.h"

#define WICEDPIN_LOW	WICED_FALSE
#define WICEDPIN_HIGHT	WICED_TRUE

#define DEMO_KEY_GPIO		(WICED_GPIO_2)

extern void demo_delay_milliseconds(u32 ms);
extern u32 demo_getSysTime_sec(void);
extern  u32 demo_getSysTime_ms(void);
extern int demo_creatUdpServer(int *fd);
extern void demo_destoryUDPServer(int fd);
extern int demo_sendUDPPacket 
	(
	int fd,
	char* buffer, 
	uint16_t buffer_length
	);
extern int demo_recv
    (
    int fd,
    char *pbuf,
    u32 *pbuflen
    );
extern void demo_factoryRestInit(void);
extern void demo_rest(void);
extern u8 demo_getPinLeve(u32 pin);



#ifdef __cplusplus
} /*extern "C" */
#endif

