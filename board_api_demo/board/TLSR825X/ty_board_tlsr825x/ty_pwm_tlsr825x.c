#include "ty_pwm.h"
#include "tl_common.h"
#include "drivers.h"

/*********************************************************************************
    PWM0   :  PA2.  PC1.  PC2.  PD5.
    PWM1   :  PA3.  PC3.
    PWM2   :  PA4.  PC4.
    PWM3   :  PB0.  PD2.
    PWM4   :  PB1.  PB4.
    PWM5   :  PB2.  PB5.
    PWM0_N :  PA0.  PB3.  PC4.	PD5.
    PWM1_N :  PC1.  PD3.
    PWM2_N :  PD4.
    PWM3_N :  PC5.
    PWM4_N :  PC0.  PC6.
    PWM5_N :  PC7.
 *********************************************************************************/

uint32_t ty_pwm_init(ty_pwm_t* p_pwm)
{
    if (p_pwm == NULL) {
        return 1;
    }
    if (p_pwm->duty < 0 || p_pwm->duty > 100) {
        return 2;
    }
    if (p_pwm->func < AS_PWM0 || p_pwm->func > AS_PWM5_N) {
        return 3;
    }

    pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
    gpio_set_func(p_pwm->pin, p_pwm->func);
    pwm_id id = (p_pwm->func < AS_PWM0_N) ? (p_pwm->func - AS_PWM0) : (p_pwm->func - AS_PWM0_N);
	pwm_set_mode(id, PWM_NORMAL_MODE);
    pwm_polo_enable(id, p_pwm->polarity);
	pwm_set_cycle_and_duty(id, (uint16_t)(p_pwm->freq * CLOCK_SYS_CLOCK_1US),
                                     (uint16_t)(p_pwm->freq * CLOCK_SYS_CLOCK_1US * p_pwm->duty / 100));

    return 0;
}

uint32_t ty_pwm_start(ty_pwm_t* p_pwm)
{
    if (p_pwm == NULL) {
        return 1;
    }
    if (p_pwm->duty < 0 || p_pwm->duty > 100) {
        return 2;
    }
    if (p_pwm->func < AS_PWM0 || p_pwm->func > AS_PWM5_N) {
        return 3;
    }

    pwm_id id = (p_pwm->func < AS_PWM0_N) ? (p_pwm->func - AS_PWM0) : (p_pwm->func - AS_PWM0_N);
    pwm_start(id);
    return 0;
}

uint32_t ty_pwm_stop(ty_pwm_t* p_pwm)
{
    if (p_pwm == NULL) {
        return 1;
    }
    if (p_pwm->func < AS_PWM0 || p_pwm->func > AS_PWM5_N) {
        return 3;
    }

    pwm_id id = (p_pwm->func < AS_PWM0_N) ? (p_pwm->func - AS_PWM0) : (p_pwm->func - AS_PWM0_N);
	pwm_stop(id);
    return 0;
}

uint32_t ty_pwm_control(ty_pwm_t* p_pwm, uint8_t cmd, void* arg)
{
    if ((p_pwm == NULL) || (arg == NULL)) {
        return 1;
    }
    if (p_pwm->func < AS_PWM0 || p_pwm->func > AS_PWM5_N) {
        return 3;
    }
    pwm_id id = (p_pwm->func < AS_PWM0_N) ? (p_pwm->func - AS_PWM0) : (p_pwm->func - AS_PWM0_N);

    switch(cmd) {
    case TY_PWM_CMD_SET_POLARITY: {
            ty_pwm_set_polarity_t* param = arg;
            p_pwm->polarity = param->polarity;
            pwm_polo_enable(id, p_pwm->polarity);
        }
        break;
    case TY_PWM_CMD_SET_FREQ: {
            ty_pwm_set_freq_t* param = arg;
            p_pwm->freq = param->freq;
            pwm_set_cycle_and_duty(id, (uint16_t)(p_pwm->freq * CLOCK_SYS_CLOCK_1US),
                                             (uint16_t)(p_pwm->freq * CLOCK_SYS_CLOCK_1US * p_pwm->duty / 100));
        }
        break;
    case TY_PWM_CMD_SET_DUTY: {
            ty_pwm_set_duty_t* param = arg;
            if (p_pwm->duty < 0 || p_pwm->duty > 100) {
                return 2;
            }
            p_pwm->duty = param->duty;
            pwm_set_cmp(id, (uint16_t)(p_pwm->freq * CLOCK_SYS_CLOCK_1US * p_pwm->duty / 100));
        }
        break;
    default:
        break;
    }

    return 0;
}

uint32_t ty_pwm_uninit(ty_pwm_t* p_pwm)
{
    uint32_t res = ty_pwm_stop(p_pwm);
    if (res) {
        return res;
    }
    gpio_set_func(p_pwm->pin, AS_GPIO);
    return 0;
}
