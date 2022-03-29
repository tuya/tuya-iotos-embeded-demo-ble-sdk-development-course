/**
****************************************************************************
* @file      ty_pwm.h
* @brief     ty_pwm
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_PWM_H__
#define __TY_PWM_H__

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
typedef enum {
    TY_PWM_CMD_SET_POLARITY = 0,
    TY_PWM_CMD_SET_FREQ,
    TY_PWM_CMD_SET_DUTY,
} ty_pwm_cmd_t;

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
typedef struct {
	GPIO_PinTypeDef pin;
    GPIO_FuncTypeDef func;
    uint8_t  polarity;
    uint32_t freq;
    uint8_t  duty;
} ty_pwm_t;

typedef struct {
    uint8_t  polarity;
} ty_pwm_set_polarity_t;

typedef struct {
    uint32_t freq;
} ty_pwm_set_freq_t;

typedef struct {
    uint8_t  duty;
} ty_pwm_set_duty_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_pwm_init    (ty_pwm_t* p_pwm);
uint32_t ty_pwm_start   (ty_pwm_t* p_pwm);
uint32_t ty_pwm_stop    (ty_pwm_t* p_pwm);
uint32_t ty_pwm_control (ty_pwm_t* p_pwm, uint8_t cmd, void* arg);
uint32_t ty_pwm_uninit  (ty_pwm_t* p_pwm);


#ifdef __cplusplus
}
#endif

#endif //__TY_PWM_H__
