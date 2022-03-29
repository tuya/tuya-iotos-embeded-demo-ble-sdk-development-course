/**
 * @file ty_pin_demo.c
 * @author lifan
 * @brief ty_pin api demo
 * @version 1.0
 * @date 2022-02-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "demo_config.h"

#if (BOARD_API_DEMO == BOARD_API_PIN)

#include "ty_pin_demo.h"
#include "ty_pin.h"
#include "tuya_ble_log.h"
#include "tuya_ble_port.h"

/***********************************************************
************************micro define************************
***********************************************************/
#if (TY_BOARD == TY_BOARD_TLSR825X)
#define KEY_PIN             GPIO_PA0
#define LED_PIN             GPIO_PD7
#else

#endif

#define KEY_SCAN_TIME_MS    10

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static tuya_ble_timer_t sg_pin_timer;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief pin timer callback
 * @param none
 * @return none
 */
void __pin_timer_cb(void)
{
    ty_pin_level_t pin_level = 0;
    ty_pin_get(KEY_PIN, &pin_level);
    ty_pin_set(LED_PIN, !pin_level);
}

/**
 * @brief ty_pin api demo init
 * @param none
 * @return none
 */
void ty_pin_demo_init(void)
{
    uint32_t res;
    /* key pin init */
    res = ty_pin_init(KEY_PIN, TY_PIN_MODE_IN_PU);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_pin_init KEY_PIN failed, error code: %d", res);
        return;
    }
    /* led pin init */
    res = ty_pin_init(LED_PIN, TY_PIN_MODE_OUT_PP_LOW);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_pin_init KEY_PIN failed, error code: %d", res);
        return;
    }
    /* timer init */
    tuya_ble_timer_create(&sg_pin_timer, KEY_SCAN_TIME_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__pin_timer_cb);
    tuya_ble_timer_start(sg_pin_timer);
}

#endif /* BOARD_API_DEMO == BOARD_API_PIN */
