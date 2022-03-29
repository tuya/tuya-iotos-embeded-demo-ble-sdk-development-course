/**
****************************************************************************
* @file      ty_i2c.h
* @brief     ty_i2c
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_I2C_H__
#define __TY_I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "board.h"

/*********************************************************************
 * CONSTANT
 */

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */
#define  USE_SOFT_I2C            0
/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_i2c_init(void);
uint32_t ty_i2c_start(void);
uint32_t ty_i2c_stop(void);
uint32_t ty_i2c_control(uint8_t cmd, void* arg);
uint32_t ty_i2c_uninit(void);
uint32_t ty_i2c_send(const uint32_t addr, const uint8_t addr_len, const uint8_t* buf, uint32_t size);
uint32_t ty_i2c_rcv(const uint32_t addr, const uint8_t addr_len, uint8_t* buf, uint32_t size);

void i2c_send_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len);
void i2c_rcv_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len);
void i2c_soft_cfg(uint8_t adderss_cmd, uint8_t reg_addr, uint8_t data);
void i2c_soft_gpio_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_delay(unsigned long tim_1us);


#ifdef __cplusplus
}
#endif

#endif //__TY_I2C_H__
