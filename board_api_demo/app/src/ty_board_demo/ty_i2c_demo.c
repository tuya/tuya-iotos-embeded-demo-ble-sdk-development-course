/**
 * @file ty_i2c_demo.c
 * @author lifan
 * @brief ty_i2c api demo
 * @version 1.0
 * @date 2022-03-02
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "demo_config.h"

#if (BOARD_API_DEMO == BOARD_API_I2C)

#include "ty_i2c_demo.h"
#include "ty_i2c.h"
#include "tuya_ble_log.h"
#include "tuya_ble_port.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Demo config */
#define I2C_API_DEMO_S              0x00            /* software I2C */
#define I2C_API_DEMO_H              0x01            /* hardware I2C */
#define I2C_API_DEMO_MODE           I2C_API_DEMO_S

#define I2C_API_DEMO_SHT3X          0x00            /* SHT3x */
#define I2C_API_DEMO_MPU6050        0x01            /* MPU6050 */
#define I2C_API_DEMO_DEV            I2C_API_DEMO_SHT3X

/* slave device config */
#if (I2C_API_DEMO_DEV == I2C_API_DEMO_SHT3X)
/* SHT3x */
#define I2C_SLAVE_ADDR              0x44
#define I2C_DAQ_TIME_MS             1000            /* 1s / 1Hz */
/* register map */
#define SHT3X_CMD_MEAS_PERI_1_H     0x2130          /* measurement: periodic 1 mps, high repeatability */
#define SHT3X_CMD_FETCH_DATA        0xE000          /* readout measurements for periodic mode */

#else
/* MPU6050 */
#define I2C_SLAVE_ADDR              0x68
#define I2C_DAQ_TIME_MS             5               /* 5ms / 200Hz */
#define MPU6050_DEV_ID              0x68
/* register map */
#define MPU6050_RA_SMPRT_DIV        0x19
#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_WHO_AM_I         0x75

#endif

/* I2C command */
#define I2C_CMD_BIT_WRITE           0
#define I2C_CMD_BIT_READ            1
#define I2C_ADDR_CMD_W              (I2C_SLAVE_ADDR << 1 | I2C_CMD_BIT_WRITE)
#define I2C_ADDR_CMD_R              (I2C_SLAVE_ADDR << 1 | I2C_CMD_BIT_READ)

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static tuya_ble_timer_t sg_daq_timer;

/***********************************************************
***********************function define**********************
***********************************************************/
#if (I2C_API_DEMO_DEV == I2C_API_DEMO_SHT3X)

/**
 * @brief check checksum
 * @param[in] data: data to be checked
 * @param[in] len: data length
 * @param[in] crc_val: crc value
 * @return check result
 */
static uint8_t __check_crc_8(uint8_t* data, uint16_t len, uint8_t crc_val)
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

    if (crc != crc_val){
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
static void __i2c_write_cmd_sht3x(uint16_t cmd, bool stop)
{
#if (I2C_API_DEMO_MODE == I2C_API_DEMO_S)
    uint8_t cmd_bytes[2];
    cmd_bytes[0] = (uint8_t)(cmd >> 8);
    cmd_bytes[1] = (uint8_t)(cmd & 0x00FF);
    i2c_start();
    i2c_send_bytes(I2C_ADDR_CMD_W, cmd_bytes, 2);
    if (stop) {
        i2c_stop();
    }
#else
    ty_i2c_send(cmd, 2, null, 0);
#endif
}

/**
 * @brief read data from SHT3x
 * @param[out] buf: data buffer
 * @param[in] len: data length
 * @return none
 */
static void __i2c_read_data_sht3x(uint8_t *buf, uint8_t len)
{
#if (I2C_API_DEMO_MODE == I2C_API_DEMO_S)
    i2c_start();
    i2c_rcv_bytes(I2C_ADDR_CMD_R, buf, len);
    i2c_stop();
#else
    ty_i2c_rcv(0, 0, buf, len);
#endif
}

#else

/**
 * @brief write register of MPU6050
 * @param[in] reg_addr: register address
 * @param[in] reg_val: value to be written
 * @return none
 */
static void __i2c_write_reg(uint8_t reg_addr, uint8_t reg_val)
{
#if (I2C_API_DEMO_MODE == I2C_API_DEMO_S)
    i2c_soft_cfg(I2C_ADDR_CMD_W, reg_addr, reg_val);
#else
    ty_i2c_send(reg_addr, 1, &reg_val, 1);
#endif
}

/**
 * @brief read register of MPU6050
 * @param[in] reg_addr: register address
 * @param[out] reg_val: register value
 * @param[in] len: register length
 * @return none
 */
static void __i2c_read_reg(uint8_t reg_addr, uint8_t *reg_val, uint8_t len)
{
#if (I2C_API_DEMO_MODE == I2C_API_DEMO_S)
    i2c_start();
    i2c_send_bytes(I2C_ADDR_CMD_W, &reg_addr, 1);
    i2c_start();
    i2c_rcv_bytes(I2C_ADDR_CMD_R, reg_val, len);
    i2c_stop();
#else
    ty_i2c_rcv(reg_addr, 1, reg_val, len);
#endif
}

#endif

/**
 * @brief data acquisition timer handler
 * @param none
 * @return none
 */
static void __daq_timer_handler(void)
{
#if (I2C_API_DEMO_DEV == I2C_API_DEMO_SHT3X)

    uint8_t buf[6];
    /* send fetch command to SHT3x */
    __i2c_write_cmd_sht3x(SHT3X_CMD_FETCH_DATA, 0);
    /* read data from SHT3x */
    __i2c_read_data_sht3x(buf, 6);
    /* check CRC value */
    if ((!__check_crc_8(buf, 2, buf[2])) ||
        (!__check_crc_8(buf+3, 2, buf[5]))) {
        TUYA_APP_LOG_ERROR("__sht3x_check_crc failed.");
        return;
    }
    /* calculate temperature and humidity */
    int32_t temp = ((int16_t)buf[0] << 8 | buf[1]) * 175 / 65535 - 45;
    uint32_t humi = ((uint16_t)buf[3] << 8 | buf[4]) * 100 / 65535;
    TUYA_APP_LOG_INFO("Temperature: %d, Humidity: %d", temp, humi);

#else

    uint8_t i;
    uint8_t tmp_buf[14];
    int16_t accel[3], gyro[3];
    /* read data from MPU6050 */
    __i2c_read_reg(MPU6050_RA_ACCEL_XOUT_H, tmp_buf, 14);
    /* calculate acceleration (g) and angular velocity (dps) */
    for (i = 0; i < 3; i++) {
        accel[i] = ((int16_t)tmp_buf[i*2] << 8) | tmp_buf[i*2+1];
        accel[i] /= 2048;
        gyro[i] = ((int16_t)tmp_buf[i*2+8] << 8) | tmp_buf[i*2+9];
        gyro[i] = gyro[i] * 10 / 164;
    }
    TUYA_APP_LOG_INFO("ax: %d,   ay: %d,   az: %d,   gx: %d,  gy: %d,  gz: %d",
                       accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);

#endif
}

/**
 * @brief ty_i2c api demo init
 * @param none
 * @return none
 */
void ty_i2c_demo_init(void)
{
    /* I2C init */
#if (I2C_API_DEMO_MODE == I2C_API_DEMO_S)
    i2c_soft_gpio_init();
#else
    ty_i2c_init();
#endif
    /* slave device init */
#if (I2C_API_DEMO_DEV == I2C_API_DEMO_SHT3X)
    /* SHT3x: start periodic measurement */
    __i2c_write_cmd_sht3x(SHT3X_CMD_MEAS_PERI_1_H, 0);
#else
    /* reset MPU6050 and exit sleep mode */
    __i2c_write_reg(MPU6050_RA_PWR_MGMT_1, 0x80);
    i2c_delay(200*1000);
    /* check communication */
    uint8_t dev_id;
    __i2c_read_reg(MPU6050_RA_WHO_AM_I, &dev_id, 1);
    if (dev_id != MPU6050_DEV_ID) {
        TUYA_APP_LOG_ERROR("MPU6050 is not found.");
        return;
    }
    /* MPU6050 init */
    __i2c_write_reg(MPU6050_RA_PWR_MGMT_1, 0x01);   /* set clock source: PLL_XGYRO */
    __i2c_write_reg(MPU6050_RA_GYRO_CONFIG, 0x03);  /* set gyroscope's full-scale range: 2000dps */
    __i2c_write_reg(MPU6050_RA_ACCEL_CONFIG, 0x03); /* set accelerometer's full-scale range: 16g */
    __i2c_write_reg(MPU6050_RA_SMPRT_DIV, 0x04);    /* set sample rate: 1kHz/(1+4) = 200Hz */
    __i2c_write_reg(MPU6050_RA_CONFIG, 0x02);       /* set DLPF */
#endif
    /* creat and start DAQ timer */
    tuya_ble_timer_create(&sg_daq_timer, I2C_DAQ_TIME_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__daq_timer_handler);
    tuya_ble_timer_start(sg_daq_timer);
}

#endif /* BOARD_API_DEMO == BOARD_API_I2C */
