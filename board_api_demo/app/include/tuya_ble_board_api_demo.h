/**
 * @file tuya_ble_board_api_demo.h
 * @author lifan
 * @brief board api demo based on BLE module
 * @version 1.0
 * @date 2022-02-17
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TUYA_BLE_BOARD_API_DEMO_H__
#define __TUYA_BLE_BOARD_API_DEMO_H__

#include "demo_config.h"

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
 * @brief board api init
 * @param[in] none
 * @return none
 */
void tuya_ble_board_api_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_BLE_BOARD_API_DEMO_H__ */
