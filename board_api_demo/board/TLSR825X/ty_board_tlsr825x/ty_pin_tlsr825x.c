#include "ty_pin.h"
#include "tl_common.h"
#include "drivers.h"

#define  GPIO_WAKEUP_MODULE_POLARITY    0
#define  WAKEUP_MODULE_GPIO             GPIO_PA0

uint32_t ty_pin_init(uint16_t pin, ty_pin_mode_t mode)
{
    gpio_set_func(pin, AS_GPIO);

    if ((mode & TY_PIN_INOUT_MASK) <= TY_PIN_IN_IRQ) {
        gpio_set_input_en(pin, 1);
        gpio_set_output_en(pin, 0);
    } else {
        gpio_set_input_en(pin, 0);
        gpio_set_output_en(pin, 1);
    }

    switch (mode) {
    case TY_PIN_MODE_IN_PU:
        gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K);
        break;
    case TY_PIN_MODE_IN_PD:
        gpio_setup_up_down_resistor(pin, PM_PIN_PULLDOWN_100K);
        break;
    case TY_PIN_MODE_IN_FL:
        gpio_setup_up_down_resistor(pin, PM_PIN_UP_DOWN_FLOAT);
        break;
    case TY_PIN_MODE_OUT_PP_LOW:
        gpio_write(pin, 0);
        break;
    case TY_PIN_MODE_OUT_PP_HIGH:
        gpio_write(pin, 1);
        break;
    default:
    	break;
    }

    #if (GPIO_WAKEUP_MODULE_POLARITY == 1)
		cpu_set_gpio_wakeup (WAKEUP_MODULE_GPIO, Level_High, 1);  // pad high wakeup deepsleep
		GPIO_WAKEUP_MODULE_LOW;
	#else
		cpu_set_gpio_wakeup (WAKEUP_MODULE_GPIO, Level_Low, 1);  // pad high wakeup deepsleep
		GPIO_WAKEUP_MODULE_HIGH;
	#endif

    return 0;
}

uint32_t ty_pin_set(uint16_t pin, ty_pin_level_t level)
{
    gpio_write(pin, level);
    return 0;
}

uint32_t ty_pin_get(uint16_t pin, ty_pin_level_t* p_level)
{
    *p_level = gpio_read(pin);
    return 0;
}

uint32_t ty_pin_control(uint16_t pin, uint8_t cmd, void* arg)
{
    return 0;
}

uint32_t ty_pin_uninit(uint16_t pin, ty_pin_mode_t mode)
{
    return 0;
}
