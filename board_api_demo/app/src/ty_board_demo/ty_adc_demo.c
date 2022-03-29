/**
 * @file ty_adc_demo.c
 * @author lifan
 * @brief ty_adc api demo
 * @version 1.0
 * @date 2022-02-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "demo_config.h"

#if (BOARD_API_DEMO == BOARD_API_ADC)

#include "ty_adc_demo.h"
#include "ty_adc.h"
#include "tuya_ble_log.h"
#include "tuya_ble_port.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define ADC_PIN             GPIO_PB6
#define ADC_SAMPLE_TIME_MS  1000

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
#if (TY_BOARD == TY_BOARD_TLSR825X)
static ty_adc_t sg_adc = {
    .pin = ADC_PIN,
    .value = 0
};
#else

#endif

static tuya_ble_timer_t sg_adc_timer;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief adc timer callback
 * @param none
 * @return none
 */
void __adc_timer_cb(void)
{
    uint32_t res = ty_adc_start(&sg_adc);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_adc_start failed, error code: %d", res);
    } else {
        TUYA_APP_LOG_INFO("adc_value is: %d mV", sg_adc.value);
    }
}

/**
 * @brief ty_adc api demo init
 * @param none
 * @return none
 */
void ty_adc_demo_init(void)
{
    uint32_t res = ty_adc_init(&sg_adc);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_adc_init failed, error code: %d", res);
        return;
    }
    tuya_ble_timer_create(&sg_adc_timer, ADC_SAMPLE_TIME_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__adc_timer_cb);
    tuya_ble_timer_start(sg_adc_timer);
}

#endif /* BOARD_API_DEMO == BOARD_API_ADC */
