/**
 * @file ty_sht3x.h
 * @author lifan
 * @brief SHT3x driver header file
 * @version 1.0
 * @date 2022-02-14
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#ifndef __TY_SHT3X_H__
#define __TY_SHT3X_H__

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
typedef uint8_t SHT3X_REPT_E;
#define REPEATAB_HIGH   0x00
#define REPEATAB_MEDIUM 0x01
#define REPEATAB_LOW    0x02

typedef uint8_t SHT3X_FREQ_E;
#define FREQ_HZ5        0x00
#define FREQ_1HZ        0x01
#define FREQ_2HZ        0x02
#define FREQ_4HZ        0x03
#define FREQ_10HZ       0x04

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief SHT3x init
 * @param[in] addr_pin_high: ADDR pin
 * @return none
 */
void ty_sht3x_init(bool addr_pin_high);

/**
 * @brief SHT3x's ALERT pin init
 * @param[in] pin: ALERT pin number
 * @param[in] active_low: ALERT pin active level
 * @return none
 */
void ty_sht3x_alert_init(int pin, bool active_low);

/**
 * @brief SHT3x's ALERT pin init
 * @param[in] pin: ALERT pin number
 * @return pin level
 */
bool ty_sht3x_read_alert(int pin);

/**
 * @brief measure single shot
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 * @param[in] temp_scale: conversion scale of temperature
 * @param[in] humi_scale: conversion scale of humidity
 * @return error code
 */
uint8_t ty_sht3x_measure_single_shot(int32_t *temp, int32_t *humi, uint8_t temp_scale, uint8_t humi_scale);

/**
 * @brief start periodic measure
 * @param[in] rept: repeatability
 * @param[in] freq: frequency
 * @return none
 */
void ty_sht3x_start_periodic_measure(SHT3X_REPT_E rept, SHT3X_FREQ_E freq);

/**
 * @brief read temperature and humidity
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 * @param[in] temp_scale: conversion scale of temperature
 * @param[in] humi_scale: conversion scale of humidity
 * @return error code
 */
uint8_t ty_sht3x_read_data(int32_t *temp, int32_t *humi, uint8_t temp_scale, uint8_t humi_scale);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TY_SHT3X_H__ */
