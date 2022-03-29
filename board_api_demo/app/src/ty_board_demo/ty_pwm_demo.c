/**
 * @file ty_pwm_demo.c
 * @author lifan
 * @brief ty_pwm api demo
 * @version 1.0
 * @date 2022-02-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "demo_config.h"

#if (BOARD_API_DEMO == BOARD_API_PWM)

#include "ty_pwm_demo.h"
#include "ty_pwm.h"
#include "tuya_ble_log.h"
#include "tuya_ble_port.h"

/***********************************************************
************************micro define************************
***********************************************************/
#if (TY_BOARD == TY_BOARD_TLSR825X)
#define PWM_PIN             GPIO_PB5
#define PWM_FUNC            AS_PWM5
#else
#define PWM_PIN             0
#endif

#define PWM_POL             0
#define PWM_POL_UPDATE      1
#define PWM_FREQ            500
#define PWM_FREQ_UPDATE     800
#define PWM_DUTY            50
#define PWM_DUTY_UPDATE     80

#define PWM_UPDATE_TIME_MS  2000

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
#if (TY_BOARD == TY_BOARD_TLSR825X)
static ty_pwm_t sg_pwm = {
    .pin = PWM_PIN,
    .func = PWM_FUNC,
    .polarity = PWM_POL,
    .freq = PWM_FREQ,
    .duty = PWM_DUTY
};
#else
static ty_pwm_t sg_pwm = {
    .pin = PWM_PIN,
    .polarity = PWM_POL,
    .freq = PWM_FREQ,
    .duty = PWM_DUTY
}
#endif

static tuya_ble_timer_t sg_pwm_timer;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief pwm timer callback
 * @param none
 * @return none
 */
void __pwm_timer_cb(void)
{
    static uint8_t s_step = 0;

    switch (s_step) {
    case 0: {
            ty_pwm_set_duty_t new_duty = {
                .duty = PWM_DUTY_UPDATE
            };
            ty_pwm_control(&sg_pwm, TY_PWM_CMD_SET_DUTY, &new_duty);
        }
        break;
    case 1: {
            ty_pwm_set_freq_t new_freq = {
                .freq = PWM_FREQ_UPDATE
            };
            ty_pwm_control(&sg_pwm, TY_PWM_CMD_SET_FREQ, &new_freq);
        }
        break;
    case 2: {
            ty_pwm_set_polarity_t new_pol = {
                .polarity = PWM_POL_UPDATE
            };
            ty_pwm_control(&sg_pwm, TY_PWM_CMD_SET_POLARITY, &new_pol);
        }
        break;
    case 3: {
            ty_pwm_stop(&sg_pwm);
            ty_pwm_uninit(&sg_pwm);
        }
        break;
    default:
        break;
    }
    if (s_step <= 3) {
        s_step++;
    }
}

/**
 * @brief ty_pwm api demo init
 * @param none
 * @return none
 */
void ty_pwm_demo_init(void)
{
    uint32_t res;
    res = ty_pwm_init(&sg_pwm);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_pwm_init failed, error code: %d", res);
        return;
    }
    res = ty_pwm_start(&sg_pwm);
    if (res) {
        TUYA_APP_LOG_ERROR("ty_pwm_start failed, error code: %d", res);
        return;
    }
    tuya_ble_timer_create(&sg_pwm_timer, PWM_UPDATE_TIME_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__pwm_timer_cb);
    tuya_ble_timer_start(sg_pwm_timer);
}

#endif /* BOARD_API_DEMO == BOARD_API_PWM */
