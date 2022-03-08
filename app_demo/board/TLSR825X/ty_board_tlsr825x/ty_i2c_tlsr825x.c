/********************************************************************************************************
 * @file     app_i2c_master.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
/*
 * i2c_master.c
 *
 *  Created on: 2018-6-15
 *      Author: Administrator
 */
#include "ty_i2c.h"
#include "tl_common.h"
#include "drivers.h"
#include "ty_oled.h"


#define  TLSR_I2C_GPIO_GROUP     I2C_GPIO_GROUP_C2C3
#define  I2C_PIN_SDA             GPIO_PC0
#define  I2C_PIN_SCL             GPIO_PC1


#define  DELAY_CNT         1

enum {
	GPIO_INPUT_MODE,
	GPIO_OUTPUT_MODE,
};


/**************************************************************************************
   8258 Demo Code Config
   1. select i2c demo:  master or slave( config it in app_config.h )
   2. if i2c slave demo,  select slave working mode: mapping mode or dma mode
   3. if i2c master demo, select the peer device i2c slave address mode:
                826x/8255 mapping mode/dma mode or other not telink's device
            here selecting peer slave device mode is for master sending address byte length

 *************************************************************************************/
#define     SLAVE_TELINK_825x_DMA_MODE      1 //slave  825x dma mode, master send 3 byte sram address(0x40000~0x4ffff)
#define     SLAVE_TELINK_82xx_MAPPING_MODE  2 //slave  826x/825x  mapping mode, master no need send address(address length is 0)
#define     SLAVE_TELINK_826x_DMA_MODE      3 //slave  826x dma mode, master send 2 byte sram address(0x8000~0xffff)
#define     SLAVE_OTHER_I2C_DEVICE			4 //slave  not telink mcu, address length may be 1 or 2 or others.
#define		PEER_SLAVE_DEVICE				SLAVE_TELINK_825x_DMA_MODE

int master_rx_index = 0;

static void i2c_sda_pin_mode_set(uint8_t mode, uint8_t level)
{
	if(mode == GPIO_INPUT_MODE)
	{
		gpio_set_input_en(I2C_PIN_SDA, 1);
		gpio_set_output_en(I2C_PIN_SDA, 0);
	}
	else if(mode == GPIO_OUTPUT_MODE)
	{
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

uint32_t ty_i2c_init(void)
{
    #if  (USE_SOFT_I2C == 0)
		//I2C pin set
		i2c_gpio_set(TLSR_I2C_GPIO_GROUP);  	//SDA/CK : C2/C3
		//slave device id 0x5C(write) 0x5D(read)
		//i2c clock 200K, only master need set i2c clock
		i2c_master_init(SLAVE_ADDRE_WRITE, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    #else
	    i2c_soft_gpio_init();
    #endif

    return 0;
}

/**
 * @description: i2c start signal
 * @param {type} none
 * @return: none
 */
uint32_t ty_i2c_start(void)
{
	#if  (USE_SOFT_I2C == 0)
	#else
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );    //SDA output mode

    i2c_scl_pin_set( 1 );
    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
	#endif
    return 0;
}

/**
 * @description: i2c stop signal
 * @param {type} none
 * @return: none
 */
uint32_t ty_i2c_stop(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 0 );     //SDA input mode

    i2c_scl_pin_set( 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

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


/**
 * @description: i2c ack func
 * @param {type} none
 * @return: none
 */
static void i2c_ack(void)
{
    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
}

/**
 * @description: i2c none ack func
 * @param {type} none
 * @return: none
 */
static void i2c_noack(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );
    i2c_sda_pin_set( 1 );

    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 0 );
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

    i2c_sda_pin_mode_set( GPIO_INPUT_MODE, 1 );/* set input and release SDA */
    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 0 );       /* put down SCL ready to cheack SCA status */
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    while( i2c_sda_pin_status_get() ) /* get ack */
    {
        cnt--;
        if( cnt==0 )
        {
            i2c_scl_pin_set( 0 );
            return false;
        }
        i2c_delay(DELAY_CNT);
    }

    i2c_scl_pin_set( 0 );
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
    i2c_scl_pin_set( 0 );
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );

    for( idx=0; idx<8; idx++ ) {
        if( data & 0x80 ) {
            i2c_sda_pin_set( 1 );
        } else {
            i2c_sda_pin_set( 0 );
        }
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set( 1 );
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set( 0 );
        i2c_delay(DELAY_CNT);

        data <<= 1;
    }
}

/**
 * @description: send bytes to i2c bus
 * @param {type} none
 * @return: none
 */
uint32_t ty_i2c_send(const uint8_t addr, const uint8_t* buf, uint32_t size)
{
	#if  (USE_SOFT_I2C == 0)
	i2c_write_series(buf[0], 0x01, (uint8_t *)&buf[1], size-1);
	#else
    uint32_t idx;
    i2c_send_byte( addr);
    i2c_wait_ack();

    for( idx=0; idx<size; idx++ ) {
        i2c_send_byte( buf[idx] );
        i2c_wait_ack();
    }
	#endif
    return 0;
}




void i2c_send_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();

    for( idx=0; idx<len; idx++ ) {
        i2c_send_byte( buff[idx] );
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
    i2c_sda_pin_mode_set( GPIO_INPUT_MODE, 1 );
    i2c_delay(25);

    for( idx=0; idx<8; idx++ ) {
        i2c_scl_pin_set( 0 );
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set( 1 );
        *data = *data << 1;
        if( i2c_sda_pin_status_get() ) {
            *data |= 1;
        }
        i2c_delay(DELAY_CNT);
    }

    i2c_scl_pin_set( 0 );
}

/**
 * @description: recive bytes from i2c bus,last byte none ack
 * @param {type} none
 * @return: none
 */
void i2c_rcv_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();

    for( idx=0; idx<len; idx++ ) {
        i2c_rcv_byte( &buff[idx] );

        if( idx<len-1 ) {
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

void i2c_soft_gpio_init(void)
{
	gpio_set_func(I2C_PIN_SDA, AS_GPIO);
	gpio_set_func(I2C_PIN_SCL, AS_GPIO);

    gpio_set_output_en(I2C_PIN_SDA, 1);
	gpio_set_input_en(I2C_PIN_SDA, 0);

	gpio_set_output_en(I2C_PIN_SCL, 1);
    gpio_set_input_en(I2C_PIN_SCL, 0);
}

void i2c_start(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );    //SDA output mode

    i2c_scl_pin_set( 1 );
    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
     
}
void i2c_stop(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 0 );     //SDA input mode

    i2c_scl_pin_set( 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);
}

void i2c_delay(unsigned long tim_1us)
{
	sleep_us(tim_1us);
}

