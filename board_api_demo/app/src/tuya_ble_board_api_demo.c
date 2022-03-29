/**
 * @file tuya_ble_board_api_demo.c
 * @author lifan
 * @brief board api demo based on BLE module
 * @version 1.0
 * @date 2022-02-18
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_ble_board_api_demo.h"
#include "tuya_ble_log.h"

#if (BOARD_API_DEMO == BOARD_API_PIN)
    #include "ty_pin_demo.h"
#elif (BOARD_API_DEMO == BOARD_API_PWM)
    #include "ty_pwm_demo.h"
#elif (BOARD_API_DEMO == BOARD_API_ADC)
    #include "ty_adc_demo.h"
#elif (BOARD_API_DEMO == BOARD_API_I2C)
    #include "ty_i2c_demo.h"
#elif (BOARD_API_DEMO == BOARD_API_FLASH)
    #include "ty_flash_demo.h"
#else /* default */
    #include "ty_pin_demo.h"
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
void tuya_ble_board_api_init(void)
{
#if (BOARD_API_DEMO == BOARD_API_PIN)
    TUYA_APP_LOG_INFO("ty_pin api demo is running.");
    ty_pin_demo_init();

#elif (BOARD_API_DEMO == BOARD_API_PWM)
    TUYA_APP_LOG_INFO("ty_pwm api demo is running.");
    ty_pwm_demo_init();

#elif (BOARD_API_DEMO == BOARD_API_ADC)
    TUYA_APP_LOG_INFO("ty_adc api demo is running.");
    ty_adc_demo_init();

#elif (BOARD_API_DEMO == BOARD_API_I2C)
    TUYA_APP_LOG_INFO("ty_i2c api demo is running.");
    ty_i2c_demo_init();

#elif (BOARD_API_DEMO == BOARD_API_FLASH)
    TUYA_APP_LOG_INFO("ty_flash api demo is running.");
    ty_flash_demo_init();

#else /* default */
    TUYA_APP_LOG_INFO("ty_pin api demo is running.");
    ty_pin_demo_init();

#endif
}
