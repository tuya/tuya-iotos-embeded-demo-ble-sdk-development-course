/**
 * @file demo_config.h
 * @author lifan
 * @brief board api demo config
 * @version 1.0
 * @date 2022-02-28
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __DEMO_CONFIG_H__
#define __DEMO_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
/* board api demo selection */
#define BOARD_API_PIN           0x00               /* api in ty_pin.h */
#define BOARD_API_PWM           0x01               /* api in ty_pwm.h */
#define BOARD_API_ADC           0x02               /* api in ty_adc.h */
#define BOARD_API_I2C           0x03               /* api in ty_i2c.h */
#define BOARD_API_FLASH         0x04               /* api in ty_flash.h */
#define BOARD_API_DEMO          BOARD_API_PIN

/* board selection */
#define TY_BOARD_TLSR825X       0x00                /* ty_board_tlsr825x */
#define TY_BOARD_NRF52832       0x01                /* ty_board_nRF52832 */
#define TY_BOARD_PHY62XX        0x02                /* ty_board_phy62xx */
#define TY_BOARD                TY_BOARD_TLSR825X

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEMO_CONFIG_H__ */
