/**
****************************************************************************
* @file      tuya_ble_product_test_demo.h
* @brief     tuya_ble_product_test_demo
* @author    suding
* @version   V1.0.0
* @date      2020-04
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TUYA_BLE_PRODUCT_TEST_DEMO_H__
#define __TUYA_BLE_PRODUCT_TEST_DEMO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "tuya_ble_stdlib.h"

/*********************************************************************
 * CONSTANT
 */
//RFU-remain_for_future_use, PID-product_id, FW-firmware, FPM-finger_print_module
#define PRODUCT_TEST_CMD_ENTER                  0xFF02
#define PRODUCT_TEST_CMD_EXIT                   0xFF03
#define PRODUCT_TEST_CMD_LED                    0x0001
#define PRODUCT_TEST_CMD_RELAY                  0x0002
#define PRODUCT_TEST_CMD_KEY                    0x0003
#define PRODUCT_TEST_CMD_SWITCH_SENSOR          0x0004
#define PRODUCT_TEST_CMD_RFU                    0x0005
#define PRODUCT_TEST_CMD_ANALOG_SENSOR          0x0006
#define PRODUCT_TEST_CMD_MOTOR                  0x0007
#define PRODUCT_TEST_CMD_BATTERY_PARAM          0x0008
#define PRODUCT_TEST_CMD_BATTERY_CALIBRATION    0x0009
#define PRODUCT_TEST_CMD_LED_RGBCW              0x000A
#define PRODUCT_TEST_CMD_BURNIN                 0x000B
#define PRODUCT_TEST_CMD_INFRARED_TX            0x000C
#define PRODUCT_TEST_CMD_INFRARED_RX_ENTER      0x000D
#define PRODUCT_TEST_CMD_INFRARED_RX_EXIT       0x000E
#define PRODUCT_TEST_CMD_WRITE_SN               0x000F
#define PRODUCT_TEST_CMD_READ_SN                0x0010
#define PRODUCT_TEST_CMD_BLE_RSSI               0x0011
#define PRODUCT_TEST_CMD_WIFI_RSSI              0x0012
#define PRODUCT_TEST_CMD_GSENSOR                0x0013
#define PRODUCT_TEST_CMD_READ_MAC               0x0014
#define PRODUCT_TEST_CMD_READ_PID               0x0015
#define PRODUCT_TEST_CMD_READ_FW_INFO           0x0016
#define PRODUCT_TEST_CMD_FPM                    0x0017

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
typedef struct 
{
    uint16_t sof;
    uint8_t  version;
    uint8_t  id;
    uint16_t len;
    uint8_t  type;
    uint16_t sub_id;
    uint8_t  value[];
} ty_product_test_cmd_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
void tuya_ble_custom_app_production_test_process(uint8_t channel, uint8_t* p_in_data, uint16_t in_len);
uint32_t tuya_ble_product_test_rsp(uint8_t channel, uint16_t cmdId, uint8_t* buf, uint16_t size);


#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_PRODUCT_TEST_DEMO_H__

