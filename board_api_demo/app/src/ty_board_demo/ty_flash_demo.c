/**
 * @file ty_flash_demo.c
 * @author lifan
 * @brief ty_flash api demo
 * @version 1.0
 * @date 2022-03-01
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "demo_config.h"

#if (BOARD_API_DEMO == BOARD_API_FLASH)

#include "ty_flash_demo.h"
#include "ty_flash.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
#define FLASH_ADDR_STA      0x034000
#define FLASH_BUF_SIZE      4

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief ty_flash api demo init
 * @param none
 * @return none
 */
void ty_flash_demo_init(void)
{
    uint8_t i;
    uint8_t flash_buf[FLASH_BUF_SIZE];
    uint8_t flash_buf_tmp[FLASH_BUF_SIZE];

    /* read flash */
    ty_flash_read(FLASH_ADDR_STA, flash_buf, FLASH_BUF_SIZE);
    for (i = 0; i < FLASH_BUF_SIZE; i++) {
        TUYA_APP_LOG_INFO("The data in %x is %02x", FLASH_ADDR_STA + 8*i, flash_buf[i]);
    }
    /* erase flash and check it */
    if (flash_buf[0] != 0xFF) {
        ty_flash_erase(FLASH_ADDR_STA, 1);
        ty_flash_read(FLASH_ADDR_STA, flash_buf_tmp, FLASH_BUF_SIZE);
        for (i = 0; i < FLASH_BUF_SIZE; i++) {
            TUYA_APP_LOG_INFO("The data in %x is %02x", FLASH_ADDR_STA + 8*i, flash_buf_tmp[i]);
        }
    }

    /* write flash and check it */
    for (i = 0; i < FLASH_BUF_SIZE; i++) {
        flash_buf[i] += (i+1);
    }
    ty_flash_write(FLASH_ADDR_STA, flash_buf, FLASH_BUF_SIZE);
    ty_flash_read(FLASH_ADDR_STA, flash_buf_tmp, FLASH_BUF_SIZE);
    for (i = 0; i < FLASH_BUF_SIZE; i++) {
        TUYA_APP_LOG_INFO("The data in %x is %02x", FLASH_ADDR_STA + 8*i, flash_buf_tmp[i]);
    }
}

#endif /* BOARD_API_DEMO == BOARD_API_FLASH */
