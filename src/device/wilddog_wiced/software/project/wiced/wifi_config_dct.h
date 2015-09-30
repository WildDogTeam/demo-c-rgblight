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

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/*
 * AP settings in this file are stored in the DCT. These
 * settings may be overwritten at manufacture when the
 * DCT is written with the final production configuration
 */

/* This is the soft AP used for device configuration */
#define CONFIG_AP_SSID       "WICED Config"
#define CONFIG_AP_PASSPHRASE ""
#define CONFIG_AP_SECURITY   WICED_SECURITY_OPEN
#define CONFIG_AP_CHANNEL    1

/* This is the soft AP available for normal operation (if used)*/
#define SOFT_AP_SSID         "WICED Device"
#define SOFT_AP_PASSPHRASE   "WICED_PASSPHRASE"
#define SOFT_AP_SECURITY     WICED_SECURITY_WPA2_AES_PSK
#define SOFT_AP_CHANNEL      1

#define CLIENT_AP_BSS_TYPE   WICED_BSS_TYPE_INFRASTRUCTURE
#define CLIENT_AP_SECURITY   WICED_SECURITY_WPA2_MIXED_PSK
#define CLIENT_AP_CHANNEL    1
#define CLIENT_AP_BAND       WICED_802_11_BAND_2_4GHZ

/* Override default country code */
#define WICED_COUNTRY_CODE    WICED_COUNTRY_UNITED_STATES

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#include "wilddog_demo_config.h"

 #ifdef __cplusplus
} /*extern "C" */
#endif

