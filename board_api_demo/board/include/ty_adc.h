/**
****************************************************************************
* @file      ty_adc.h
* @brief     ty_adc
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_ADC_H__
#define __TY_ADC_H__

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
#pragma pack(1)
typedef struct {
	GPIO_PinTypeDef pin;
    uint32_t value;
} ty_adc_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_adc_init(ty_adc_t* p_adc);
uint32_t ty_adc_start(ty_adc_t* p_adc);
uint32_t ty_adc_stop(ty_adc_t* p_adc);
uint32_t ty_adc_control(ty_adc_t* p_adc, uint8_t cmd, void* arg);
uint32_t ty_adc_uninit(ty_adc_t* p_adc);


#ifdef __cplusplus
}
#endif

#endif //__TY_ADC_H__
