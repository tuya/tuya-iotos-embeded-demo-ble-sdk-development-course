#include "tuya_ble_sdk_demo.h"
#include "tuya_ble_ota.h"
#include "ty_ble.h"
#include "ty_rtc.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_unix_time.h"
#include "tuya_ble_sdk_test.h"
#include "tuya_ble_feature_weather.h"
#include "tuya_ble_bulk_data_demo.h"

#include "tuya_ble_sensor_rht_demo.h"


/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static const char auth_key_test[]  = TY_DEVICE_AUTH_KEY;
static const char device_id_test[] = TY_DEVICE_DID;

//tuya_ble_sdk init param
static tuya_ble_device_param_t tuya_ble_device_param = {
    .use_ext_license_key = 0, //1-info in tuya_ble_sdk_demo.h, 0-auth info
    .device_id_len       = 0,//DEVICE_ID_LEN, //DEVICE_ID_LEN-info in tuya_ble_sdk_demo.h, 0-auth info
    .p_type              = TUYA_BLE_PRODUCT_ID_TYPE_PID,
#if ( TUYA_BLE_PROD_SUPPORT_OEM_TYPE == TUYA_BLE_PROD_OEM_TYPE_0_5 )
    .product_id_len      = 0,//8,//8-TUYA_BLE_PROD_OEM_TYPE_NONE(customized product) 0-TUYA_BLE_PROD_OEM_TYPE_0_5
#else
    .product_id_len      = 8,
#endif
    .adv_local_name_len  = 4,
    .firmware_version    = TY_DEVICE_FVER_NUM,
    .hardware_version    = TY_DEVICE_HVER_NUM,
};

static tuya_ble_timer_t disconnect_timer;
static tuya_ble_timer_t update_conn_param_timer;

tuya_ble_app_master_result_handler_t tuya_ble_app_master_result_handler;

/*********************************************************************
 * VARIABLE
 */
demo_dp_t g_cmd;
demo_dp_t g_rsp;

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
static void tuya_ble_sdk_callback(tuya_ble_cb_evt_param_t* event)
{
//    TUYA_APP_LOG_INFO("callback: %02x", event->evt);
    
    switch(event->evt)
    {
        case TUYA_BLE_CB_EVT_CONNECTE_STATUS: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_DEVICE_STATE, (void*)&event->connect_status, sizeof(uint8_t));
#endif
            
            if(event->connect_status == BONDING_CONN) {
                TUYA_APP_LOG_INFO("bonding and connecting");
                tuya_ble_update_conn_param_timer_start();
                tuya_net_proc_ble_conn();
            }
        } break;
        
        case TUYA_BLE_CB_EVT_DP_DATA_RECEIVED: {
#if TUYA_BLE_SDK_TEST
        	tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITHOUT_RESPONSE, event->dp_received_data.p_data, event->dp_received_data.data_len);

            tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_WRITE, event->dp_received_data.p_data, event->dp_received_data.data_len);
#endif
            tuya_net_proc_dp_recv(event->dp_received_data.p_data, event->dp_received_data.data_len);
        } break;
        
        case TUYA_BLE_CB_EVT_DP_DATA_SEND_RESPONSE: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_dp_report_handler();
            tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_RSP, &event->dp_send_response_data.status, sizeof(uint8_t));
#endif
        } break;
        
        case TUYA_BLE_CB_EVT_DP_DATA_WITH_TIME_SEND_RESPONSE: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_dp_report_handler();
            tuya_ble_sdk_test_send(TY_UARTV_CMD_DP_RSP, &event->dp_with_time_send_response_data.status, sizeof(uint8_t));
#endif
        } break;
        
        case TUYA_BLE_CB_EVT_TIME_STAMP: {
            uint32_t timestamp_s = tuya_ble_ascii_to_int((void*)event->timestamp_data.timestamp_string, 10);
            uint32_t timestamp_ms = tuya_ble_ascii_to_int((void*)(event->timestamp_data.timestamp_string+10), 3);
            
            uint64_t timestamp = 0;
            timestamp = timestamp_s*1000 + timestamp_ms;
            
            ty_rtc_set_time(timestamp_s);
            
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_TIME_STAMP - time_zone: %d", event->timestamp_data.time_zone);
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_TIME_STAMP - timestamp: %d", timestamp_s);
            
#if TUYA_BLE_SDK_TEST
            tuya_ble_time_struct_data_t data = {0};;
            tuya_ble_utc_sec_2_mytime(timestamp_s, &data, false);
            
            tuya_ble_time_noraml_data_t normal_data = {0};
            memcpy(&normal_data, &data, sizeof(tuya_ble_time_struct_data_t));
            normal_data.time_zone = event->timestamp_data.time_zone;
            
            tuya_ble_sdk_test_get_time_rsp(&normal_data);
#endif
        } break;
        
        case TUYA_BLE_CB_EVT_TIME_NORMAL:
        case TUYA_BLE_CB_EVT_APP_LOCAL_TIME_NORMAL: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_get_time_rsp(&event->time_normal_data);
#endif
        } break;
        
        case TUYA_BLE_CB_EVT_UNBOUND: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_unbind_mode_rsp(0);
#endif
            tuya_net_proc_ble_unbound();
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_UNBOUND");
        } break;
        
        case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_unbind_mode_rsp(1);
#endif
            tuya_net_proc_ble_unbound();
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_ANOMALY_UNBOUND");
        } break;
        
        case TUYA_BLE_CB_EVT_DEVICE_RESET: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_unbind_mode_rsp(2);
#endif
            
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_DEVICE_RESET");
        } break;
        
        case TUYA_BLE_CB_EVT_UNBIND_RESET_RESPONSE: {
#if TUYA_BLE_SDK_TEST
            if(event->reset_response_data.type == RESET_TYPE_UNBIND) {
                if(event->reset_response_data.status == 0) {
                    tuya_ble_sdk_test_unbind_mode_rsp(3);
                }
            } else if(event->reset_response_data.type == RESET_TYPE_FACTORY_RESET) {
                if(event->reset_response_data.status == 0) {
                    tuya_ble_sdk_test_unbind_mode_rsp(4);
                }
            }
#endif
            
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_UNBIND_RESET_RESPONSE");
        } break;
        
        case TUYA_BLE_CB_EVT_DP_QUERY: {
//            TUYA_APP_LOG_HEXDUMP_DEBUG("TUYA_BLE_CB_EVT_DP_QUERY", event->dp_query_data.p_data, event->dp_query_data.data_len);
        } break;
        
        case TUYA_BLE_CB_EVT_OTA_DATA: {
            tuya_ble_ota_handler(&event->ota_data);
        } break;
        
        case TUYA_BLE_CB_EVT_BULK_DATA: {
            TUYA_APP_LOG_INFO("TUYA_BLE_CB_EVT_BULK_DATA");
            tuya_ble_bulk_data_demo_handler(&event->bulk_req_data);
        } break;
        
        case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_send(TY_UARTV_CMD_PASSTHROUGH_WRITE, event->ble_passthrough_data.p_data, event->ble_passthrough_data.data_len);
#endif
        } break;
        
        case TUYA_BLE_CB_EVT_WEATHER_DATA_REQ_RESPONSE: {
#if TUYA_BLE_SDK_TEST
            uint8_t  rsp_data[2] = {1};
            uint32_t rsp_len = 2;
            rsp_data[1] = event->weather_req_response_data.status;
            tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_WEATHER, rsp_data, rsp_len);
            TUYA_APP_LOG_INFO("received weather data request response result code =%d",event->weather_req_response_data.status);
#endif
        } break;
            
        case TUYA_BLE_CB_EVT_WEATHER_DATA_RECEIVED: {
#if TUYA_BLE_SDK_TEST
            tuya_ble_wd_object_t *object;
            uint16_t object_len = 0;
            for (;;) {
                object = (tuya_ble_wd_object_t *)(event->weather_received_data.p_data + object_len);

                TUYA_APP_LOG_DEBUG("recvived weather data, n_days=[%d] key=[0x%08x] val_type=[%d] val_len=[%d]", \
                                object->n_day, object->key_type, object->val_type, object->value_len); 
                TUYA_APP_LOG_HEXDUMP_DEBUG("vaule :", (uint8_t *)object->vaule, object->value_len);	

                // TODO .. YOUR JOBS 
                
                object_len += (sizeof(tuya_ble_wd_object_t) + object->value_len);
                if (object_len >= event->weather_received_data.data_len)
                    break;
            }
            
            tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_WEATHER, event->weather_received_data.p_data, event->weather_received_data.data_len);
#endif
        } break;
            
        case TUYA_BLE_CB_EVT_UPDATE_LOGIN_KEY_VID: {
        } break;
        
        default: {
            TUYA_APP_LOG_INFO("tuya_ble_sdk_callback unknown event type 0x%04x", event->evt);
        } break;
    }
}

/*********************************************************
FN: 
*/
void tuya_ble_sdk_demo_init(void)
{
    if(tuya_ble_device_param.use_ext_license_key)
    {
        memcpy(tuya_ble_device_param.device_id,       device_id_test, DEVICE_ID_LEN);
        memcpy(tuya_ble_device_param.auth_key,        auth_key_test,  AUTH_KEY_LEN);
        memcpy(tuya_ble_device_param.mac_addr_string, TY_DEVICE_MAC,  MAC_STRING_LEN);
    }

    memcpy(tuya_ble_device_param.product_id,      TY_DEVICE_PID,  tuya_ble_device_param.product_id_len);
    memcpy(tuya_ble_device_param.adv_local_name,  TY_DEVICE_NAME, tuya_ble_device_param.adv_local_name_len);
    tuya_ble_sdk_init(&tuya_ble_device_param);
    
    tuya_ble_callback_queue_register(tuya_ble_sdk_callback);
    
    tuya_ble_ota_init();
    tuya_ble_disconnect_and_reset_timer_init();
    tuya_ble_update_conn_param_timer_init();
    
    //extern tuya_ble_parameters_settings_t tuya_ble_current_para;
    //TUYA_APP_LOG_HEXDUMP_INFO("auth key", tuya_ble_current_para.auth_settings.auth_key, AUTH_KEY_LEN);
    //TUYA_APP_LOG_HEXDUMP_INFO("device id", tuya_ble_current_para.auth_settings.device_id, DEVICE_ID_LEN);

    tuya_ble_sensor_rht_init();
}

/*********************************************************
FN:  1-info in tuya_ble_sdk_demo.h, 0-auth info
*/
uint8_t tuya_ble_license_key_type(void){
    return tuya_ble_device_param.use_ext_license_key;
}

/*********************************************************
FN: 
*/
static void tuya_ble_app_data_process(int32_t evt_id, void *data)
{
    custom_evt_data_t* custom_data = data;
    
    switch (evt_id)
    {
        case APP_EVT_0: {
        } break;
        
        case APP_EVT_1: {
        } break;
        
        case APP_EVT_2: {
        } break;
        
        case APP_EVT_3: {
        } break;
        
        case APP_EVT_4: {
        } break;
        
        default: {
        } break;
    }
    
    if(custom_data != NULL) {
        tuya_ble_free((void*)custom_data);
    }
}

/*********************************************************
FN: no data
*/
void tuya_ble_custom_evt_send(custom_evtid_t evtid)
{
    tuya_ble_custom_evt_t custom_evt;
    
    custom_evt.evt_id = evtid;
    custom_evt.data = NULL;
    custom_evt.custom_event_handler = tuya_ble_app_data_process;
    
    tuya_ble_custom_event_send(custom_evt);
}

/*********************************************************
FN: 
*/
void tuya_ble_custom_evt_send_with_data(custom_evtid_t evtid, void* buf, uint32_t size)
{
    custom_evt_data_t* custom_data = tuya_ble_malloc(sizeof(custom_evt_data_t) + size);
    if(custom_data) {
        tuya_ble_custom_evt_t custom_evt;
        
        custom_data->len = size;
        memcpy(custom_data->value, buf, size);
        
        custom_evt.evt_id = evtid;
        custom_evt.data = custom_data;
        custom_evt.custom_event_handler = tuya_ble_app_data_process;
        
        tuya_ble_custom_event_send(custom_evt);
    } else {
        TUYA_APP_LOG_ERROR("tuya_ble_custom_evt_send_with_data: malloc failed");
    }
}

/*********************************************************
FN: 
*/
static void tuya_ble_disconnect_and_reset_timer_cb(tuya_ble_timer_t timer)
{
    tuya_ble_gap_disconnect();
    tuya_ble_device_delay_ms(100);
    tuya_ble_device_reset();
}

/*********************************************************
FN: 
*/
static void tuya_ble_update_conn_param_timer_cb(tuya_ble_timer_t timer)
{
    ty_ble_set_conn_param(TY_CONN_INTERVAL_MIN, TY_CONN_INTERVAL_MAX, 0, 6000);
}

/*********************************************************
FN: 
*/
void tuya_ble_disconnect_and_reset_timer_init(void)
{
    tuya_ble_timer_create(&disconnect_timer, 1000, TUYA_BLE_TIMER_SINGLE_SHOT, tuya_ble_disconnect_and_reset_timer_cb);
}

/*********************************************************
FN: 
*/
void tuya_ble_update_conn_param_timer_init(void)
{
    tuya_ble_timer_create(&update_conn_param_timer, 1000, TUYA_BLE_TIMER_SINGLE_SHOT, tuya_ble_update_conn_param_timer_cb);
}

/*********************************************************
FN: 
*/
void tuya_ble_disconnect_and_reset_timer_start(void)
{
    tuya_ble_timer_start(disconnect_timer);
}

/*********************************************************
FN: 
*/
void tuya_ble_update_conn_param_timer_start(void)
{
    tuya_ble_timer_start(update_conn_param_timer);
}








