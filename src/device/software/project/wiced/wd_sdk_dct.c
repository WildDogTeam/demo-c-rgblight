/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "wiced_framework.h"
#include "wd_sdk_dct.h"
#include "wilddog_demo_config.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

//#define DEVICE_ID    "D123456"   /* 48 characters */
#define USER_ID       ""   /* up to 50 characters*/

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
 *               Variable Definitions
 ******************************************************/

DEFINE_APP_DCT(user_dct_data_t)
{
    .isBinded           = 0,
    .configType         = SMARTCONFIG_TYPE_COOEE,
    .device_id          = YOUR_DEVICE_NAME,
    .user_id            = USER_ID,
};

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
