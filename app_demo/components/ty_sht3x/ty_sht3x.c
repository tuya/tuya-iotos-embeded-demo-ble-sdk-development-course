/**
 * @file ty_sht3x.c
 * @author lifan
 * @brief SHT3x driver source file
 * @version 1.0
 * @date 2022-02-14
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "ty_sht3x.h"
#include "ty_i2c.h"
#include "ty_pin.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* I2C device address */
#define SHT3X_DEV_ADDR_A            0x44    /* ADDR (pin 2) connected to logic low */
#define SHT3X_DEV_ADDR_B            0x45    /* ADDR (pin 2) connected to logic high */

/* I2C R/W command */
#define I2C_CMD_BIT_WRITE           0
#define I2C_CMD_BIT_READ            1

/* SHT3x command */
#define SHT3X_CMD_MEAS_CLOCKSTR_H   0x2C06  /* measurement: clock stretching, high repeatability */
#define SHT3X_CMD_MEAS_CLOCKSTR_M   0x2C0D  /* measurement: clock stretching, medium repeatability */
#define SHT3X_CMD_MEAS_CLOCKSTR_L   0x2C10  /* measurement: clock stretching, low repeatability */
#define SHT3X_CMD_MEAS_POLLING_H    0x2400  /* measurement: polling, high repeatability */
#define SHT3X_CMD_MEAS_POLLING_M    0x240B  /* measurement: polling, medium repeatability */
#define SHT3X_CMD_MEAS_POLLING_L    0x2416  /* measurement: polling, low repeatability */
#define SHT3X_CMD_MEAS_PERI_05_H    0x2032  /* measurement: periodic 0.5 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_05_M    0x2024  /* measurement: periodic 0.5 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_05_L    0x202F  /* measurement: periodic 0.5 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_1_H     0x2130  /* measurement: periodic 1 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_1_M     0x2126  /* measurement: periodic 1 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_1_L     0x212D  /* measurement: periodic 1 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_2_H     0x2236  /* measurement: periodic 2 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_2_M     0x2220  /* measurement: periodic 2 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_2_L     0x222B  /* measurement: periodic 2 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_4_H     0x2334  /* measurement: periodic 4 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_4_M     0x2322  /* measurement: periodic 4 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_4_L     0x2329  /* measurement: periodic 4 mps, low repeatability */
#define SHT3X_CMD_MEAS_PERI_10_H    0x2737  /* measurement: periodic 10 mps, high repeatability */
#define SHT3X_CMD_MEAS_PERI_10_M    0x2721  /* measurement: periodic 10 mps, medium repeatability */
#define SHT3X_CMD_MEAS_PERI_10_L    0x272A  /* measurement: periodic 10 mps, low repeatability */
#define SHT3X_CMD_FETCH_DATA        0xE000  /* readout measurements for periodic mode */
#define SHT3X_CMD_ART               0x2B32  /* activate ART */
#define SHT3X_CMD_BREAK             0x3093  /* stop periodic data acquisition mode */
#define SHT3X_CMD_GENEREAL_RESET    0x0006  /* general reset */
#define SHT3X_CMD_SOFT_RESET        0x30A2  /* soft reset */
#define SHT3X_CMD_READ_STATUS       0xF32D  /* read status register */
#define SHT3X_CMD_CLEAR_STATUS      0x3041  /* clear status register */
#define SHT3X_CMD_HEATER_ENABLE     0x306D  /* enabled heater */
#define SHT3X_CMD_HEATER_DISABLE    0x3066  /* disable heater */
#define SHT3X_CMD_R_AL_LIM_LS       0xE102  /* read alert limits, low set */
#define SHT3X_CMD_R_AL_LIM_LC       0xE109  /* read alert limits, low clear */
#define SHT3X_CMD_R_AL_LIM_HS       0xE11F  /* read alert limits, high set */
#define SHT3X_CMD_R_AL_LIM_HC       0xE114  /* read alert limits, high clear */
#define SHT3X_CMD_W_AL_LIM_HS       0x611D  /* write alert limits, high set */
#define SHT3X_CMD_W_AL_LIM_HC       0x6116  /* write alert limits, high clear */
#define SHT3X_CMD_W_AL_LIM_LC       0x610B  /* write alert limits, low clear */
#define SHT3X_CMD_W_AL_LIM_LS       0x6100  /* write alert limits, low set */
#define SHT3X_CMD_READ_SERIALNBR    0x3780  /* read serial number */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
uint8_t g_dev_addr = SHT3X_DEV_ADDR_A;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief SHT3x init
 * @param[in] addr_pin_high: ADDR pin
 * @return none
 */
void ty_sht3x_init(bool addr_pin_high)
{
    /* I2C init */
    ty_i2c_init();
    /* set device address */
    if (addr_pin_high) {
        g_dev_addr = SHT3X_DEV_ADDR_B;
    }
}

/**
 * @brief SHT3x's ALERT pin init
 * @param[in] pin: ALERT pin number
 * @param[in] active_low: ALERT pin active level
 * @return none
 */
void ty_sht3x_alert_init(int pin, bool active_low)
{
    if (active_low) {
        ty_pin_init(pin, TY_PIN_MODE_IN_PU);
    } else {
        ty_pin_init(pin, TY_PIN_MODE_IN_PD);
    }
}

/**
 * @brief SHT3x's ALERT pin init
 * @param[in] pin: ALERT pin number
 * @return pin level
 */
bool ty_sht3x_read_alert(int pin)
{
    bool pin_level;
    ty_pin_get(pin, &pin_level);
    return pin_level;
}

/**
 * @brief get CRC8 value
 * @param[in] data: data to be calculated
 * @param[in] len: data length
 * @return CRC8 value
 */
static uint8_t get_crc_8(uint8_t* data, uint16_t len)
{
    uint8_t i;
    uint8_t crc = 0xFF;

    while (len--) {
        crc ^= *data;
        for (i = 8; i > 0; --i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
        data++;
    }
    return crc;
}

/**
 * @brief check checksum
 * @param[in] data: data to be checked
 * @param[in] len: data length
 * @param[in] crc_val: crc value
 * @return check result
 */
static uint8_t __sht3x_check_crc(uint8_t* data, uint16_t len, uint8_t crc_val)
{
    if (get_crc_8(data, len) != crc_val){
        return 0;
    }
    return 1;
}

/**
 * @brief write command to SHT3x
 * @param[in] cmd: command
 * @param[in] stop: whether a stop signal needs to be sent
 * @return none
 */
static void __sht3x_write_cmd(uint16_t cmd, bool stop)
{
    uint8_t cmd_bytes[2];
    cmd_bytes[0] = (uint8_t)(cmd >> 8);
    cmd_bytes[1] = (uint8_t)(cmd & 0x00FF);

    i2c_start();
    i2c_send_bytes((g_dev_addr << 1) | I2C_CMD_BIT_WRITE, cmd_bytes, 2);
    if (stop) {
        i2c_stop();
    }
}

/**
 * @brief read bytes from SHT3x
 * @param[out] buffer: data buffer
 * @param[in] len: data length
 * @return none
 */
static void __sht3x_read_bytes(uint8_t *buffer, uint8_t len)
{
    i2c_start();
    i2c_rcv_bytes((g_dev_addr << 1) | I2C_CMD_BIT_READ, buffer, len);
    i2c_stop();
}

/**
 * @brief calculate temperature
 * @param[in] raw_data: raw data of temperature
 * @param[in] scale: conversion scale of temperature
 * @return temperature in degrees Celsius
 */
static int32_t __sht3x_calc_temp(int16_t raw_data, uint8_t scale)
{
    int32_t gain = 1;
    while(scale--) {
        gain *= 10;
    }
    return (gain * 175 * (int32_t)raw_data / 65535 - gain * 45);
}

/**
 * @brief calculate humidity
 * @param[in] raw_data: raw data of humidity
 * @param[in] scale: conversion scale of humidity
 * @return humidity in %RT
 */
static int32_t __sht3x_calc_humi(uint16_t raw_data, uint8_t scale)
{
    int32_t gain = 1;
    while(scale--) {
        gain *= 10;
    }
    return (gain * 100 * (int32_t)raw_data / 65535);
}

/**
 * @brief read temperature and humidity
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 * @param[in] temp_scale: conversion scale of temperature
 * @param[in] humi_scale: conversion scale of humidity
 * @return error code
 */
uint8_t __sht3x_read_data(int32_t *temp, int32_t *humi, uint8_t temp_scale, uint8_t humi_scale)
{
    uint8_t buf[6];
    /* read data from SHT3x */
    __sht3x_read_bytes(buf, 6);
    /* check CRC value */
    if ((!__sht3x_check_crc(buf, 2, buf[2])) ||
        (!__sht3x_check_crc(buf+3, 2, buf[5]))) {
        return 0;
    }
    /* calculate temperature and humidity */
    *temp = __sht3x_calc_temp(((int16_t)buf[0] << 8) | buf[1], temp_scale);
    *humi = __sht3x_calc_humi(((int16_t)buf[3] << 8) | buf[4], humi_scale);
    return 1;
}

/**
 * @brief measure single shot
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 * @param[in] temp_scale: conversion scale of temperature
 * @param[in] humi_scale: conversion scale of humidity
 * @return error code
 */
uint8_t ty_sht3x_measure_single_shot(int32_t *temp, int32_t *humi, uint8_t temp_scale, uint8_t humi_scale)
{
    __sht3x_write_cmd(SHT3X_CMD_MEAS_CLOCKSTR_H, 1);
    i2c_delay(20000);
    return __sht3x_read_data(temp, humi, temp_scale, humi_scale);
}

/**
 * @brief start periodic measure
 * @param[in] rept: repeatability
 * @param[in] freq: frequency
 * @return none
 */
void ty_sht3x_start_periodic_measure(SHT3X_REPT_E rept, SHT3X_FREQ_E freq)
{
    switch (rept) {
    case REPEATAB_HIGH:
        switch (freq) {
        case FREQ_HZ5:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_05_H, 0);
            break;
        case FREQ_1HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_1_H, 0);
            break;
        case FREQ_2HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_2_H, 0);
            break;
        case FREQ_4HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_4_H, 0);
            break;
        case FREQ_10HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_10_H, 0);
            break;
        default:
            break;
        }
        break;
    case REPEATAB_MEDIUM:
        switch (freq) {
        case FREQ_HZ5:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_05_M, 0);
            break;
        case FREQ_1HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_1_M, 0);
            break;
        case FREQ_2HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_2_M, 0);
            break;
        case FREQ_4HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_4_M, 0);
            break;
        case FREQ_10HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_10_M, 0);
            break;
        default:
            break;
        }
        break;
    case REPEATAB_LOW:
        switch (freq) {
        case FREQ_HZ5:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_05_L, 0);
            break;
        case FREQ_1HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_1_L, 0);
            break;
        case FREQ_2HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_2_L, 0);
            break;
        case FREQ_4HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_4_L, 0);
            break;
        case FREQ_10HZ:
            __sht3x_write_cmd(SHT3X_CMD_MEAS_PERI_10_L, 0);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/**
 * @brief read data from SHT3x
 * @param[out] temp: temperature value
 * @param[out] humi: humidity value
 * @param[in] temp_scale: conversion scale of temperature
 * @param[in] humi_scale: conversion scale of humidity
 * @return error code
 */
uint8_t ty_sht3x_read_data(int32_t *temp, int32_t *humi, uint8_t temp_scale, uint8_t humi_scale)
{
    __sht3x_write_cmd(SHT3X_CMD_FETCH_DATA, 0);
    return __sht3x_read_data(temp, humi, temp_scale, humi_scale);
}

#if 0
/**
 * @brief activate ART
 * @param none
 * @return none
 */
static void __sht3x_activate_art(void)
{
    __sht3x_write_cmd(SHT3X_CMD_ART, 0);
}

/**
 * @brief stop periodic data acquisition mode
 * @param none
 * @return none
 */
static void __sht3x_stop_periodic_measure(void)
{
    __sht3x_write_cmd(SHT3X_CMD_BREAK, 1);
}

/**
 * @brief soft reset
 * @param none
 * @return none
 */
static void __sht3x_soft_reset(void)
{
    __sht3x_write_cmd(SHT3X_CMD_SOFT_RESET, 1);
}

/**
 * @brief soft reset
 * @param none
 * @return none
 */
static void __sht3x_general_reset(void)
{
    uint8_t cmd = (uint8_t)SHT3X_CMD_GENEREAL_RESET;
    i2c_start();
    i2c_send_bytes((g_dev_addr << 1) | I2C_CMD_BIT_WRITE, &cmd, 1);
}

/**
 * @brief set heater
 * @param none
 * @return none
 */
static void __sht3x_set_heater(bool enable)
{
    if (enable) {
        __sht3x_write_cmd(SHT3X_CMD_HEATER_ENABLE, 1);
    } else {
        __sht3x_write_cmd(SHT3X_CMD_HEATER_DISABLE, 1);
    }
}

/**
 * @brief get status
 * @param[out] status: status code
 * @return error code
 */
static uint8_t __sht3x_get_status(uint16_t *status)
{
    uint8_t buf[3];
    __sht3x_write_cmd(SHT3X_CMD_READ_STATUS, 0);
    i2c_delay(3000);
    __sht3x_read_bytes(buf, 3);
    if (!__sht3x_check_crc(buf, 2, buf[2])) {
        return 0;
    }
    *status = ((uint16_t)buf[0] << 8) | buf[1];
    return 1;
}

/**
 * @brief clear status
 * @param none
 * @return none
 */
static void __sht3x_clear_status(void)
{
    __sht3x_write_cmd(SHT3X_CMD_CLEAR_STATUS, 1);
}
#endif
