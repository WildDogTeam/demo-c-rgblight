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


int start_smartconfig(void)
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


