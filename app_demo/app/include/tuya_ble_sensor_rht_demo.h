/**
 * @file tuya_ble_sensor_rht_demo.h
 * @author lifan
 * @brief tuya sensor/RH-T application based on BLE module
 * @version 1.0
 * @date 2022-02-14
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TUYA_BLE_SENSOR_RHT_DEMO_H__
#define __TUYA_BLE_SENSOR_RHT_DEMO_H__

#include "tuya_ble_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief ble connected process
 * @param none
 * @return none
 */
void tuya_net_proc_ble_conn(void);

/**
 * @brief ble unbound process
 * @param none
 * @return none
 */
void tuya_net_proc_ble_unbound(void);

/**
 * @brief received DP data process
 * @param[in] dp_data: dp data array
 * @param[in] dp_len: dp data length
 * @return none
 */
void tuya_net_proc_dp_recv(uint8_t *dp_data, const uint16_t dp_len);

/**
 * @brief RH-T sensor application init
 * @param none
 * @return none
 */
void tuya_ble_sensor_rht_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_BLE_SENSOR_RHT_DEMO_H__ */
