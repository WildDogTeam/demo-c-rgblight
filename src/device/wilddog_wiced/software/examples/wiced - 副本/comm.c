#include "JSON.h"

extern    char cmd[32];
extern    char token[256];
extern    char userId[32];
extern    char devId[32];

wiced_result_t comm( wiced_json_object_t* json_object )
{
#if 0
    WPRINT_APP_INFO(("object_string:%s\n", json_object->object_string));
    WPRINT_APP_INFO(("object_string_length:%d\n", json_object->object_string_length));
    WPRINT_APP_INFO(("value type:%d\n" , json_object->value_type));
    WPRINT_APP_INFO(("value:%s\n", json_object->value));
    WPRINT_APP_INFO(("value length:%d\n", json_object->value_length));
#endif

    /*parse cmd*/
    if( 0 == strncmp("cmd", json_object->object_string, json_object->object_string_length))
    {
        strncpy( cmd, json_object->value, json_object->value_length);
    }

    if( 0 == strncmp("token", json_object->object_string, json_object->object_string_length))
    {
        strncpy( token, json_object->value, json_object->value_length);
    }

    if( 0 == strncmp("userId", json_object->object_string, json_object->object_string_length))
    {
        int ii, kk;
        for(ii =0, kk = 0; ii < json_object->value_length; ii++)
        {

            if(json_object->value[ii] == ':')
            {
                userId[kk++] = '\\';
                userId[kk++] = json_object->value[++ii];
            }
            else
                userId[kk++] = json_object->value[ii];
        }
        strncpy( userId, json_object->value, json_object->value_length);
    }

    if( 0 == strncmp("devId", json_object->object_string, json_object->object_string_length))
    {
        strncpy( devId, json_object->value, json_object->value_length);
    }

#if 0
    WPRINT_APP_INFO(("cmd:%s\n", cmd));
    WPRINT_APP_INFO(("token:%s\n", token));
    WPRINT_APP_INFO(("userId:%s\n", userId));
    WPRINT_APP_INFO(("devId:%s\n", devId));
#endif
}
