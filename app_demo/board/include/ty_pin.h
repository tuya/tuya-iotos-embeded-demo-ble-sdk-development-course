/**
****************************************************************************
* @file      ty_pin.h
* @brief     ty_pin
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_PIN_H__
#define __TY_PIN_H__

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
//2bit
#define TY_PIN_INIT_LOW               ((uint16_t)(0x01 << 0))
#define TY_PIN_INIT_HIGH              ((uint16_t)(0x02 << 0))
#define TY_PIN_INIT_MASK              ((uint16_t)(0x03 << 0))

//3bit
#define TY_PIN_IN                     ((uint16_t)(0x01 << 2))
#define TY_PIN_IN_FL                  ((uint16_t)(0x02 << 2))
#define TY_PIN_IN_IRQ                 ((uint16_t)(0x03 << 2))
#define TY_PIN_OUT_PP                 ((uint16_t)(0x04 << 2))
#define TY_PIN_OUT_OD                 ((uint16_t)(0x05 << 2))
#define TY_PIN_INOUT_MASK             ((uint16_t)(0x07 << 2))

//3bit
#define TY_PIN_IRQ_RISE               ((uint16_t)(0x01 << 5))
#define TY_PIN_IRQ_FALL               ((uint16_t)(0x02 << 5))
#define TY_PIN_IRQ_RISE_FALL          ((uint16_t)(0x03 << 5))
#define TY_PIN_IRQ_LOW                ((uint16_t)(0x04 << 5))
#define TY_PIN_IRQ_HIGH               ((uint16_t)(0x05 << 5))
#define TY_PIN_IRQ_MASK               ((uint16_t)(0x07 << 5))

//4bit 
#define TY_PIN_PULL_UP                ((uint16_t)(0x01 << 9))
#define TY_PIN_PULL_DOWN              ((uint16_t)(0x02 << 9))
#define TY_PIN_PULL_NONE              ((uint16_t)(0x03 << 9))
#define TY_PIN_MODE_MASK              ((uint16_t)(0x0F << 9))

typedef enum {
    //PU  ->  pull up
    //PD  ->  pull dowm
    //FL  ->  floating
    //PP  ->  push pull
    //OD  ->  open drain
    //hiz ->  high-impedance level

    TY_PIN_MODE_IN_PU               = TY_PIN_IN     | TY_PIN_PULL_UP,
    TY_PIN_MODE_IN_PD               = TY_PIN_IN     | TY_PIN_PULL_DOWN,
    TY_PIN_MODE_IN_FL               = TY_PIN_IN_FL,

    TY_PIN_MODE_IN_IRQ_RISE         = TY_PIN_IN_IRQ | TY_PIN_IRQ_RISE       | TY_PIN_PULL_UP, 
    TY_PIN_MODE_IN_IRQ_FALL         = TY_PIN_IN_IRQ | TY_PIN_IRQ_FALL       | TY_PIN_PULL_UP,
    TY_PIN_MODE_IN_IRQ_RISE_FALL    = TY_PIN_IN_IRQ | TY_PIN_IRQ_RISE_FALL  | TY_PIN_PULL_UP,
    TY_PIN_MODE_IN_IRQ_LOW          = TY_PIN_IN_IRQ | TY_PIN_IRQ_LOW        | TY_PIN_PULL_UP,
    TY_PIN_MODE_IN_IRQ_HIGH         = TY_PIN_IN_IRQ | TY_PIN_IRQ_HIGH       | TY_PIN_PULL_UP,
    
    TY_PIN_MODE_OUT_PP_LOW          = TY_PIN_OUT_PP | TY_PIN_INIT_LOW,
    TY_PIN_MODE_OUT_PP_HIGH         = TY_PIN_OUT_PP | TY_PIN_INIT_HIGH,

    TY_PIN_MODE_OUT_PP_PU_LOW       = TY_PIN_OUT_PP | TY_PIN_PULL_UP        | TY_PIN_INIT_LOW,
    TY_PIN_MODE_OUT_PP_PU_HIGH      = TY_PIN_OUT_PP | TY_PIN_PULL_UP        | TY_PIN_INIT_HIGH,

    TY_PIN_MODE_OUT_PP_PD_LOW       = TY_PIN_OUT_PP | TY_PIN_PULL_DOWN      | TY_PIN_INIT_LOW,
    TY_PIN_MODE_OUT_PP_PD_HIGH      = TY_PIN_OUT_PP | TY_PIN_PULL_DOWN      | TY_PIN_INIT_HIGH,

    TY_PIN_MODE_OUT_OD_LOW          = TY_PIN_OUT_OD | TY_PIN_INIT_LOW,
    TY_PIN_MODE_OUT_OD_HIZ          = TY_PIN_OUT_OD | TY_PIN_INIT_HIGH,

    TY_PIN_MODE_OUT_OD_PU_LOW       = TY_PIN_OUT_OD | TY_PIN_PULL_UP        | TY_PIN_INIT_LOW,
    TY_PIN_MODE_OUT_OD_PU_HIGH      = TY_PIN_OUT_OD | TY_PIN_PULL_UP        | TY_PIN_INIT_HIGH,
} ty_pin_mode_t;

typedef enum {
    TY_PIN_LOW = 0,
    TY_PIN_HIGH
} ty_pin_level_t;

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_pin_init(uint16_t pin, ty_pin_mode_t mode);
uint32_t ty_pin_set(uint16_t pin, ty_pin_level_t level);
uint32_t ty_pin_get(uint16_t pin, ty_pin_level_t* p_level);
uint32_t ty_pin_control(uint16_t pin, uint8_t cmd, void* arg);
uint32_t ty_pin_uninit(uint16_t pin, ty_pin_mode_t mode);


#ifdef __cplusplus
}
#endif

#endif //__TY_PIN_H__
