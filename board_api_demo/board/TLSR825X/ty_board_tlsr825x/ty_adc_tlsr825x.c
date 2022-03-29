#include "ty_adc.h"
#include "tl_common.h"
#include "drivers.h"

uint32_t ty_adc_init(ty_adc_t* p_adc)
{
	adc_init();
	adc_base_init(p_adc->pin);
	adc_power_on_sar_adc(1);
	return 0;
}

uint32_t ty_adc_start(ty_adc_t* p_adc)
{
	p_adc->value = adc_sample_and_get_result();
	return 0;
}

uint32_t ty_adc_stop(ty_adc_t* p_adc)
{
	return 0;
}

uint32_t ty_adc_control(ty_adc_t* p_adc, uint8_t cmd, void* arg)
{
	return 0;
}

uint32_t ty_adc_uninit(ty_adc_t* p_adc)
{
	adc_power_on_sar_adc(0);
	// in order to reduce consumption ,user may need config the io state of adc(input,pull high or down or float)
	return 0;
}
