#include "ty_i2c.h"
#include "tl_common.h"
#include "drivers.h"


#define TLSR_I2C_GPIO_GROUP     I2C_GPIO_GROUP_C0C1
#define I2C_PIN_SDA             GPIO_PC0
#define I2C_PIN_SCL             GPIO_PC1
#define SLAVE_ADDR_WRITE        (0x44 << 1 | 0x00)  // (0x68 << 1 | 0x00)

#define DELAY_CNT               1

enum {
	GPIO_INPUT_MODE,
	GPIO_OUTPUT_MODE,
};

uint32_t ty_i2c_init(void)
{
    i2c_gpio_set(TLSR_I2C_GPIO_GROUP);
    i2c_master_init(SLAVE_ADDR_WRITE, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)));
    return 0;
}

uint32_t ty_i2c_start(void)
{
    return 0;
}

uint32_t ty_i2c_stop(void)
{
    return 0;
}

uint32_t ty_i2c_control(uint8_t cmd, void* arg)
{
    return 0;
}

uint32_t ty_i2c_uninit(void)
{
   return 0;
}

uint32_t ty_i2c_send(const uint32_t addr, const uint8_t addr_len, const uint8_t* buf, uint32_t size)
{
	i2c_write_series(addr, addr_len, buf, size);
    return 0;
}

uint32_t ty_i2c_rcv(const uint32_t addr, const uint8_t addr_len, uint8_t* buf, uint32_t size)
{
	i2c_read_series(addr, addr_len, buf, size);
    return 0;
}

static void i2c_sda_pin_mode_set(uint8_t mode, uint8_t level)
{
	if (mode == GPIO_INPUT_MODE) {
		gpio_set_input_en(I2C_PIN_SDA, 1);
		gpio_set_output_en(I2C_PIN_SDA, 0);
	} else if(mode == GPIO_OUTPUT_MODE)	{
        gpio_set_input_en(I2C_PIN_SDA, 0);
        gpio_set_output_en(I2C_PIN_SDA, 1);
	}
}

static void i2c_sda_pin_set(uint8_t level)
{
	gpio_write(I2C_PIN_SDA, level);
}

static void i2c_scl_pin_set(uint8_t level)
{
	gpio_write(I2C_PIN_SCL, level);
}

static uint8_t i2c_sda_pin_status_get(void)
{
    return (!gpio_read(I2C_PIN_SDA))?0:1;
}

static void i2c_ack(void)
{
    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_mode_set(GPIO_OUTPUT_MODE, 0);
    i2c_sda_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set(1);
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);
}

static void i2c_noack(void)
{
    i2c_sda_pin_mode_set(GPIO_OUTPUT_MODE, 1);
    i2c_sda_pin_set(1);

    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set(1);
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);
}

/**
 * @description: i2c wait ack
 * @param {type} none
 * @return: rev ack return true else return false
 */
static uint8_t i2c_wait_ack(void)
{
    uint8_t cnt = 50;

    i2c_sda_pin_mode_set(GPIO_INPUT_MODE, 1);/* set input and release SDA */
    i2c_sda_pin_set(1);
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set(0);       /* put down SCL ready to cheack SCA status */
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set(1);
    i2c_delay(DELAY_CNT);

    while (i2c_sda_pin_status_get()) { /* get ack */
        cnt--;
        if (cnt == 0) {
            i2c_scl_pin_set(0);
            return false;
        }
        i2c_delay(DELAY_CNT);
    }

    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);
    return true;
}

/**
 * @description: send one byte to i2c bus
 * @param {uint8_t} data send to i2c
 * @return: none
 */
static void i2c_send_byte(uint8_t data)
{
    uint8_t idx = 0;
    i2c_scl_pin_set(0);
    i2c_sda_pin_mode_set(GPIO_OUTPUT_MODE, 1);

    for (idx = 0; idx < 8; idx++) {
        if (data & 0x80) {
            i2c_sda_pin_set(1);
        } else {
            i2c_sda_pin_set(0);
        }
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set(1);
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set(0);
        i2c_delay(DELAY_CNT);

        data <<= 1;
    }
}

void i2c_soft_gpio_init(void)
{
	gpio_set_func(I2C_PIN_SDA, AS_GPIO);
	gpio_set_func(I2C_PIN_SCL, AS_GPIO);

    gpio_set_output_en(I2C_PIN_SDA, 1);
	gpio_set_input_en(I2C_PIN_SDA, 0);

	gpio_set_output_en(I2C_PIN_SCL, 1);
    gpio_set_input_en(I2C_PIN_SCL, 0);
}

void i2c_send_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte(adderss_cmd);
    i2c_wait_ack();

    for (idx = 0; idx < len; idx++) {
        i2c_send_byte(buff[idx]);
        i2c_wait_ack();
    }
}

/**
 * @description: recive one byte from i2c bus
 * @param {type} none
 * @return: none
 */
void i2c_rcv_byte(uint8_t *data)
{
    uint8_t idx;
    i2c_sda_pin_mode_set(GPIO_INPUT_MODE, 1);
    i2c_delay(25);

    for (idx = 0; idx < 8; idx++) {
        i2c_scl_pin_set(0);
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set(1);
        *data = *data << 1;
        if (i2c_sda_pin_status_get()) {
            *data |= 1;
        }
        i2c_delay(DELAY_CNT);
    }

    i2c_scl_pin_set(0);
}

/**
 * @description: recive bytes from i2c bus,last byte none ack
 * @param {type} none
 * @return: none
 */
void i2c_rcv_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte(adderss_cmd);
    i2c_wait_ack();

    for (idx = 0; idx < len; idx++) {
        i2c_rcv_byte(&buff[idx]);

        if (idx < len-1) {
            i2c_ack();
        } else {
            i2c_noack();
        }
    }
}

void i2c_soft_cfg(uint8_t adderss_cmd, uint8_t reg_addr, uint8_t data)
{
    i2c_start();
    i2c_send_byte(adderss_cmd);
    i2c_wait_ack();
    i2c_send_byte(reg_addr);
    i2c_wait_ack();
    i2c_send_byte(data);
    i2c_wait_ack();
    i2c_stop();
}

void i2c_start(void)
{
    i2c_sda_pin_mode_set(GPIO_OUTPUT_MODE,1);

    i2c_scl_pin_set(1);
    i2c_sda_pin_set(1);
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);
}

void i2c_stop(void)
{
    i2c_sda_pin_mode_set(GPIO_OUTPUT_MODE, 0);

    i2c_scl_pin_set(0);
    i2c_sda_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set(1);
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set(1);
    i2c_delay(DELAY_CNT);
}

void i2c_delay(unsigned long tim_1us)
{
	sleep_us(tim_1us);
}
