/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
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

#define MAX_DEVICE_ID_LEN (31) /* Maximum Device ID length                 */
#define MAX_USER_LEN      (31)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef enum SMARTCONFIG_TYPE
{
    SMARTCONFIG_TYPE_COOEE,
    SMARTCONFIG_TYPE_AKISS,
    SMARTCONFIG_TYPE_XIAOMI,
    SMARTCONFIG_TYPE_QQCONNECT,
}SmartConfig_T;


typedef struct
{
    unsigned long       configType;
    unsigned long       isBinded;
    char                device_id[MAX_DEVICE_ID_LEN + 1]; /* With terminating null */
    char                user_id[MAX_USER_LEN + 1];
} user_dct_data_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
