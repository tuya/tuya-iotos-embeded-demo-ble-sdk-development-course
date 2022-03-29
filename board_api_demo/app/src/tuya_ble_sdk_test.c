#include "tuya_ble_sdk_test.h"
#include "ty_pin.h"
#include "ty_rtc.h"
#include "ty_ble.h"
#include "ty_pwm.h"
#include "ty_adc.h"
#include "ty_uart.h"
#include "ty_i2c.h"
#include "ty_spi.h"
#include "ty_system.h"
#include "ty_oled.h"
#include "tuya_ble_port.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_unix_time.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_feature_weather.h"
#include "tuya_ble_bulk_data_demo.h"
#include "tuya_ble_mem.h"




#if TUYA_BLE_SDK_TEST

/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */
#pragma pack(1)
typedef struct {
    uint8_t  type;
    uint8_t  len;
    uint16_t value[16];
} test_pid_t;

typedef struct {
    uint32_t firmware;
    uint32_t hardware;
} test_version_t;

typedef struct {
    uint32_t min;
    uint32_t max;
} test_adv_interval_t;

typedef struct {
    uint32_t min;
    uint32_t max;
} test_conn_interval_t;

typedef struct {
    int32_t cycle;
    uint16_t len;
    uint8_t  buf[TUYA_BLE_SEND_MAX_DP_DATA_LEN];
} test_dp_report_t;

typedef struct {
    bool    flag;
    uint8_t mode;
} test_enter_sleep_t;

typedef struct {
    test_pid_t pid;
    test_version_t version;
    test_adv_interval_t adv_interval;
    test_conn_interval_t conn_interval;
    test_dp_report_t dp_report;
    test_enter_sleep_t enter_sleep;
    uint16_t timezone;
} test_param_t;
#pragma pack()

/*********************************************************************
 * LOCAL VARIABLE
 */
_attribute_no_ret_bss_ static uint8_t tx_buffer[560] = {0x66, 0xAA, 0x00, 0xF2};
static tuya_ble_timer_t enter_sleep_timer;

static test_param_t test_param;
static uint32_t test_sn = 0;
static ty_adc_t test_adc = {0};
static ty_pwm_t test_pwm = {0};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */
static void ty_uart_virtual_enter_sleep_timer_cb(tuya_ble_timer_t timer);



extern void tuya_i2c_test(void);
/*********************************************************
FN: 
*/
extern void spi_master_test(unsigned char  *spi_master_tx_buff, unsigned char *spi_master_rx__buff,uint16_t  data_len);
uint8_t spi_tx_buf[16];
uint8_t spi_rx_buf[16];
void tuya_ble_sdk_test_init(void)
{

    ty_pin_init(0, 0);
    ty_spi_init();

    // i2c test init
    ty_i2c_init();
    ty_i2c_start();

    // pwm test init
    test_pwm.pin = BOARD_PWM_PIN;
    test_pwm.polarity = 0;
    test_pwm.freq = 1000;
    test_pwm.duty = 50;
    test_pwm.id = PWM4_ID;
    ty_pwm_init(&test_pwm);

    // adc test init
    test_adc.pin = GPIO_PB5;
    test_adc.channel = BOARD_ADC_CHANNEL;
    ty_adc_init(&test_adc);

    test_param.pid.type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    test_param.pid.len = 8;
    memcpy(test_param.pid.value, TY_DEVICE_PID, test_param.pid.len);
    
    test_param.version.firmware = TY_DEVICE_FVER_NUM;
    test_param.version.hardware = TY_DEVICE_HVER_NUM;
    
    test_param.adv_interval.min = TY_ADV_INTERVAL;
    test_param.adv_interval.max = TY_ADV_INTERVAL;
    test_param.conn_interval.min = TY_CONN_INTERVAL_MIN;
    test_param.conn_interval.max = TY_CONN_INTERVAL_MAX;
    
    test_param.enter_sleep.flag = false;
    test_param.enter_sleep.mode = 0;
    
    tuya_ble_timer_create(&enter_sleep_timer, 200, TUYA_BLE_TIMER_SINGLE_SHOT, ty_uart_virtual_enter_sleep_timer_cb);
    
    tuya_ble_sdk_test_send(TY_UARTV_CMD_SHAKE_HAND, NULL, 0);
}

/*********************************************************
FN: 
*/
void tuya_ble_app_sdk_test_process(uint8_t channel, uint8_t *p_in_data, uint16_t in_len)
{
    TUYA_APP_LOG_HEXDUMP_INFO("test_cmd", p_in_data, in_len);
    
    tuya_ble_sdk_test_cmd_t* cmd = (void*)p_in_data;
    tuya_ble_inverted_array((void*)&cmd->len, sizeof(uint16_t));
    tuya_ble_inverted_array((void*)&cmd->sub_id, sizeof(uint16_t));
    
    if((cmd->type != 3) || (cmd->len < 3)){
        return;
    }
    
    uint16_t data_len = cmd->len - 3;
    
    uint8_t  rsp_data[256] = {0};
    uint32_t rsp_len = 0;
    
    switch (cmd->sub_id)
    {
        case TY_UARTV_CMD_SHAKE_HAND: {
            //nothing
        } break;

        case TY_UARTV_CMD_SET_PID: {
            if(tuya_ble_device_update_product_id(cmd->value[0], cmd->value[1], &cmd->value[2]) == TUYA_BLE_SUCCESS) {
                test_param.pid.type = cmd->value[0];
                test_param.pid.len = cmd->value[1];
                memcpy(test_param.pid.value, &cmd->value[2], test_param.pid.len);
                
                rsp_data[rsp_len] = 0;
            } else {
                rsp_data[rsp_len] = 1;
            }
            
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_PID: {
            rsp_data[rsp_len] = test_param.pid.type;
            rsp_len += 1;
            
            rsp_data[rsp_len] = test_param.pid.len;
            rsp_len += 1;
            
            memcpy(&rsp_data[rsp_len], test_param.pid.value, test_param.pid.len);
            rsp_len += test_param.pid.len;
        } break;

        case TY_UARTV_CMD_SET_AUTH_INFO: {
            tuya_ble_gap_addr_t addr;
            addr.addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;
            
            tuya_ble_hexstr2hex(&cmd->value[0], 12, addr.addr);
            
            if(tuya_ble_storage_write_auth_key_device_id_mac(&cmd->value[12+DEVICE_ID_LEN], AUTH_KEY_LEN, \
                                                             &cmd->value[12], DEVICE_ID_LEN, \
                                                             addr.addr, 6, \
                                                             &cmd->value[0], 12, NULL, 0) == TUYA_BLE_SUCCESS)
            {
                tuya_ble_gap_addr_set(&addr);
                rsp_data[rsp_len] = 0;
            } else {
                rsp_data[rsp_len] = 1;
            }
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_AUTH_INFO: {
            extern tuya_ble_parameters_settings_t tuya_ble_current_para;
            
            memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.mac_string, MAC_STRING_LEN);
            rsp_len += MAC_STRING_LEN;
            memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.device_id, DEVICE_ID_LEN);
            rsp_len += DEVICE_ID_LEN;
            memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.auth_key, AUTH_KEY_LEN);
            rsp_len += AUTH_KEY_LEN;
        } break;

        case TY_UARTV_CMD_SET_VERSION: {
            test_param.version.firmware = (cmd->value[0]<<24) + (cmd->value[1]<<16) + (cmd->value[2]<<8) + cmd->value[3];
            test_param.version.hardware = (cmd->value[4]<<24) + (cmd->value[5]<<16) + (cmd->value[6]<<8) + cmd->value[7];
            tuya_ble_set_device_version(test_param.version.firmware, test_param.version.hardware);
            
            rsp_data[rsp_len] = 0;
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_VERSION: {
            rsp_data[0] = test_param.version.firmware >> 24;
            rsp_data[1] = test_param.version.firmware >> 16;
            rsp_data[2] = test_param.version.firmware >> 8;
            rsp_data[3] = test_param.version.firmware;
            rsp_data[4] = test_param.version.hardware >> 24;
            rsp_data[5] = test_param.version.hardware >> 16;
            rsp_data[6] = test_param.version.hardware >> 8;
            rsp_data[7] = test_param.version.hardware;
            rsp_len += 8;
        } break;

        case TY_UARTV_CMD_SET_ADV_INTERVAL: {
            uint16_t adv_interval_min;
            uint16_t adv_interval_max;
            adv_interval_min = (cmd->value[0]<<8) + cmd->value[1];
            adv_interval_max = (cmd->value[2]<<8) + cmd->value[3];
            
            ty_ble_adv_param_t param;
            param.adv_interval_min = adv_interval_min;
            param.adv_interval_max = adv_interval_max;
            param.adv_type = 0x01;
            if(ty_ble_set_adv_param(&param) == TUYA_BLE_SUCCESS) {
                test_param.adv_interval.min = adv_interval_min;
                test_param.adv_interval.max = adv_interval_max;
                
                rsp_data[rsp_len] = 0;
            } else {
                rsp_data[rsp_len] = 1;
            }
            
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_ADV_INTERVAL: {
            rsp_data[0] = test_param.adv_interval.min >> 8;
            rsp_data[1] = test_param.adv_interval.min;
            rsp_data[2] = test_param.adv_interval.max >> 8;
            rsp_data[3] = test_param.adv_interval.max;
            rsp_len += 4;
        } break;

        case TY_UARTV_CMD_SET_CONN_INTERVAL: {
            uint16_t conn_interval_min;
            uint16_t conn_interval_max;
            conn_interval_min = (cmd->value[0]<<8) + cmd->value[1];
            conn_interval_max = (cmd->value[2]<<8) + cmd->value[3];
            
            if(ty_ble_set_conn_param(conn_interval_min, conn_interval_max, 0, 6000) == TUYA_BLE_SUCCESS) {
                test_param.conn_interval.min = conn_interval_min;
                test_param.conn_interval.max = conn_interval_max;
                
                rsp_data[rsp_len] = 0;
            } else {
                rsp_data[rsp_len] = 1;
            }
            
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_CONN_INTERVAL: {
            rsp_data[0] = test_param.conn_interval.min >> 8;
            rsp_data[1] = test_param.conn_interval.min;
            rsp_data[2] = test_param.conn_interval.max >> 8;
            rsp_data[3] = test_param.conn_interval.max;
            rsp_len += 4;
        } break;

        case TY_UARTV_CMD_PASSTHROUGH_REPORT: {
            rsp_data[rsp_len] = tuya_ble_data_passthrough(cmd->value, data_len);
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_TIME_SYNC: {
            rsp_data[rsp_len] = tuya_ble_time_req(cmd->value[0]);
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_DP_REPORT: {
            if(cmd->value[0] == 0) {
                rsp_data[rsp_len] = tuya_ble_dp_data_send(test_sn++, cmd->value[1], cmd->value[2], cmd->value[3], &cmd->value[4], data_len-4);
            } else if(cmd->value[0] == 1) {
                if(cmd->value[4] == 0) {
                    rsp_data[rsp_len] = tuya_ble_dp_data_with_time_send(test_sn++, cmd->value[2], cmd->value[4], &cmd->value[5], &cmd->value[18], data_len-18);
                } else {
                    rsp_data[rsp_len] = tuya_ble_dp_data_with_time_send(test_sn++, cmd->value[2], cmd->value[4], &cmd->value[5], &cmd->value[9], data_len-9);
                }
            } else if(cmd->value[0] == 2) {
                uint8_t* tmp_buf = tuya_ble_malloc(TUYA_BLE_SEND_MAX_DATA_LEN);
                if(tmp_buf) {
                    uint16_t dp_data_len = (cmd->value[6]<<8) + cmd->value[7];
                    memcpy(tmp_buf, &cmd->value[4], 4);
                    
                    for(uint32_t idx=0; idx<dp_data_len; idx++) {
                        tmp_buf[4+idx] = idx;
                    }
                    
                    rsp_data[rsp_len] = tuya_ble_dp_data_send(test_sn++, cmd->value[1], cmd->value[2], cmd->value[3], tmp_buf, dp_data_len+4);
                    
                    tuya_ble_free(tmp_buf);
                } else {
                    rsp_data[rsp_len] = 1;
                    TUYA_APP_LOG_ERROR("TY_UARTV_CMD_DP_REPORT tuya_ble_malloc fail");
                }
            }
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_DP_REPORT_PERIOD: {
            uint32_t cycle = (cmd->value[0]<<24) + (cmd->value[1]<<16) + (cmd->value[2]<<8) + cmd->value[3];
            
            test_param.dp_report.cycle = cycle;
            test_param.dp_report.len = data_len-4;
            memcpy(test_param.dp_report.buf, &cmd->value[4], test_param.dp_report.len);
            
            uint8_t* buf = test_param.dp_report.buf;
            uint32_t len = test_param.dp_report.len;
            if(buf[0] == 0) {
                tuya_ble_dp_data_send(test_sn++, buf[1], buf[2], buf[3], &buf[4], len-4);
            } else {
                if(buf[4] == 0) {
                    tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[18], len-18);
                } else {
                    tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[9], len-9);
                }
            }
            
            rsp_data[rsp_len] = 0;
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_SET_BULKDATA: {
            uint32_t cycle = (cmd->value[0]<<24) + (cmd->value[1]<<16) + (cmd->value[2]<<8) + cmd->value[3];
            uint32_t timestep = (cmd->value[4]<<24) + (cmd->value[5]<<16) + (cmd->value[6]<<8) + cmd->value[7];
            
            rsp_data[rsp_len] = tuya_ble_bulk_data_generation(timestep, &cmd->value[8], data_len-8);
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_SET_CONN_REQUEST: {
            rsp_data[rsp_len] = tuya_ble_adv_data_connecting_request_set(cmd->value[0]);
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_SET_DEVICE_RESET: {
            if(cmd->value[0] == 0) {
                rsp_data[rsp_len] = tuya_ble_device_unbind();
            } else if(cmd->value[0] == 1) {
                rsp_data[rsp_len] = tuya_ble_device_factory_reset();
            } else if(cmd->value[0] == 2) {
                tuya_ble_disconnect_and_reset_timer_start();
                rsp_data[rsp_len] = 0;
            }
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_SET_SLEEP_MODE: {
            test_param.enter_sleep.mode = cmd->value[0];
            
            tuya_ble_timer_start(enter_sleep_timer);
            
            rsp_data[rsp_len] = 0;
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_DEVICE_STATE: {
            rsp_data[rsp_len] = tuya_ble_connect_status_get();
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_GET_TIME: {
            uint32_t timestamp = 0;
            ty_rtc_get_time(&timestamp);
            
            tuya_ble_time_struct_data_t data;
            tuya_ble_utc_sec_2_mytime(timestamp, &data, false);
            
            rsp_data[0] = data.nYear-2000;
            rsp_data[1] = data.nMonth;
            rsp_data[2] = data.nDay;
            rsp_data[3] = data.nHour;
            rsp_data[4] = data.nMin;
            rsp_data[5] = data.nSec;
            rsp_data[6] = data.DayIndex;
            rsp_data[7] = test_param.timezone >> 8;
            rsp_data[8] = test_param.timezone;
            rsp_len += 9;
        } break;

        case TY_UARTV_CMD_GET_WEATHER: {
            uint32_t combine_type = (cmd->value[1]<<24) + (cmd->value[2]<<16) + (cmd->value[3]<<8) + cmd->value[4];
            rsp_data[0] = 0;
            rsp_data[1] = tuya_ble_feature_weather_data_request_with_location(cmd->value[0], combine_type, cmd->value[5]);
            rsp_len += 2;
        } break;

        case TY_UARTV_CMD_GET_RAND: {
            uint8_t rand_buf[32] = {0};
            uint8_t len = cmd->value[0];
            tuya_ble_rand_generator(rand_buf, len);
            
            memcpy(rsp_data, rand_buf, len);
            rsp_len += len;
        } break;

        case TY_UARTV_CMD_DP_WRITE: {
        } break;

        case TY_UARTV_CMD_DP_RSP: {
        } break;

        case TY_UARTV_CMD_UNBIND_MODE: {
        } break;

        case TY_UARTV_CMD_PWM: {
            uint32_t freq = (cmd->value[0]<<24) + (cmd->value[1]<<16) + (cmd->value[2]<<8) + cmd->value[3];
            uint32_t duty = cmd->value[4];
            
            test_pwm.freq = freq;
            test_pwm.duty = duty;
            ty_pwm_start(&test_pwm);
            
            rsp_data[rsp_len] = 0;
            rsp_len += 1;
        } break;

        case TY_UARTV_CMD_ADC: {
            ty_adc_start(&test_adc);
            
            uint32_t adc_value = test_adc.value;

            rsp_data[0] = adc_value >> 24;
            rsp_data[1] = adc_value >> 16;
            rsp_data[2] = adc_value >> 8;
            rsp_data[3] = adc_value;
            rsp_len += 4;
        } break;

        case TY_UARTV_CMD_SPI: {
            ty_spi_enable();
            ty_spi_readWriteData(&cmd->value[0], rsp_data, data_len);
            ty_spi_disable();
            
            rsp_len += data_len;
        } break;

        case TY_UARTV_CMD_IIC: {
            if(cmd->value[0] == 0) {
                ty_oled_init();
            } else {
                ty_oled_clear();
            }
            rsp_data[rsp_len] = 0;
            rsp_len += 1;
        } break;

        default: {
        } break;
    }
    
    tuya_ble_sdk_test_send(cmd->sub_id, rsp_data, rsp_len);
}

/*********************************************************
FN: 
*/
uint32_t tuya_ble_sdk_test_send(uint16_t cmdId, uint8_t* buf, uint16_t size)
{
    uint32_t len = 4;
    
    tx_buffer[len] = (size+3)>>8;
    len++;
    tx_buffer[len] = (size+3)&0xFF;
    len++;
    
    tx_buffer[len] = 0x03;
    len++;
    
    tx_buffer[len] = cmdId>>8;
    len++;
    tx_buffer[len] = cmdId&0xFF;
    len++;
    
    if(size > 0) {
        memcpy(&tx_buffer[len], buf, size);
        len += size;
    }
    
    tx_buffer[len] = tuya_ble_check_sum(tx_buffer, len);
    len += 1;
    
    tuya_ble_production_test_asynchronous_response(0, tx_buffer, len);
    
//    TUYA_APP_LOG_HEXDUMP_INFO(tx_buffer, len);
    return 0;
}

/*********************************************************
FN: 
*/
static void ty_uart_virtual_enter_sleep_timer_cb(tuya_ble_timer_t timer)
{
    if(test_param.enter_sleep.mode == 0) {
        ty_ble_stop_adv();
    }

    ty_rtc_uninit();
    ty_uart_uninit();
    ty_uart2_uninit();
    
    ty_oled_clear();
    ty_i2c_uninit();

    ty_pwm_uninit(&test_pwm);
    ty_adc_uninit(&test_adc);
    
    ty_system_wdt_uninit();

    if(test_param.enter_sleep.mode == 0) {
    	ty_system_enter_sleep();
    }else{
    	ty_system_enter_suspend();
    }
    
    test_param.enter_sleep.flag = true;
}

/*********************************************************
FN: 
*/
void tuya_ble_sdk_test_wake_up_handler(void)
{
    if(test_param.enter_sleep.flag) {
        test_param.enter_sleep.flag = false;
        
        ty_system_exit_sleep();
        
        ty_system_wdt_init();
        
        ty_rtc_init();
        ty_uart_init();
        ty_uart2_init();
        ty_i2c_init();
        ty_i2c_start();
        
        test_pwm.pin = BOARD_PWM_PIN;
        test_pwm.polarity = 0;
        test_pwm.freq = 1000;
        test_pwm.duty = 50;
        ty_pwm_init(&test_pwm);
        test_adc.pin = BOARD_ADC_PIN;
        test_adc.channel = BOARD_ADC_CHANNEL;
        ty_adc_init(&test_adc);
    }
    ty_ble_start_adv();
}

/*********************************************************
FN: 
*/
void tuya_ble_sdk_test_dp_report_handler(void)
{
    test_param.dp_report.cycle--;
//    TUYA_APP_LOG_INFO("test_param.dp_report.cycle: %d", test_param.dp_report.cycle);
    if(test_param.dp_report.cycle > 0) {
        uint8_t* buf = test_param.dp_report.buf;
        uint32_t len = test_param.dp_report.len;
        if(buf[0] == 0) {
            tuya_ble_dp_data_send(test_sn++, buf[1], buf[2], buf[3], &buf[4], len-4);
        } else {
            if(buf[4] == 0) {
                tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[18], len-18);
            } else {
                tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[9], len-9);
            }
        }
    }
}

/*********************************************************
FN: 
*/
void tuya_ble_sdk_test_get_time_rsp(tuya_ble_time_noraml_data_t* data)
{
    test_param.timezone = data->time_zone;
    
    uint8_t  rsp_data[10] = {0};
    uint32_t rsp_len = 9;
    
    if(data->nYear < 200) {
        rsp_data[0] = data->nYear;
    } else {
        rsp_data[0] = data->nYear-2000;
    }
    rsp_data[1] = data->nMonth;
    rsp_data[2] = data->nDay;
    rsp_data[3] = data->nHour;
    rsp_data[4] = data->nMin;
    rsp_data[5] = data->nSec;
    rsp_data[6] = data->DayIndex;
    rsp_data[7] = test_param.timezone >> 8;
    rsp_data[8] = test_param.timezone;
    
    tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_TIME, rsp_data, rsp_len);
}

/*********************************************************
FN: 
*/
void tuya_ble_sdk_test_unbind_mode_rsp(uint8_t mode)
{
    uint8_t tmp_mode = mode;
    tuya_ble_sdk_test_send(TY_UARTV_CMD_UNBIND_MODE, &tmp_mode, sizeof(uint8_t));
}

#endif









