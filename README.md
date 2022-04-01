# Tuya Bluetooth Low Energy SDK Development Tutorial

<br>

## Overview

This demo works with the [Bluetooth Low Energy (LE) SDK Development Tutorial](https://developer.tuya.com/en/docs/iot/bluetooth-le-sdk-development?id=Kbe6do0lc2wc2) (currently, this documentation is only available in Chinese).

- `app_demo`: used for [Application Development](https://developer.tuya.com/en/docs/iot/application-development?id=Kbe6embsa0wtu) (currently, this documentation is only available in Chinese).
- `board_api_demo`: used for [Board API Reference](https://developer.tuya.com/en/docs/iot/application-development?id=Kbe6embsa0wtu) (currently, this documentation is only available in Chinese).

<br>

## Development environment

Before you get started with the demo, you must have the following:

- Tuya Sandwich Bluetooth Low Energy SoC Board (BTU)
- Tuya Sandwich Temperature and Humidity Sensor Board (SHT30-DIS)
- USB to TTL converter and the debugger software
- The production software (Cloud Module Burning Authorization Platform)
- Telink burning board, Telink IDE, and Telink BDT
- Computer (Windows 10)
- SDK (`tuya-ble-sdk-demo-project-tlsr8253`)
- Smartphone with the Smart Life app installed

<br>

## Directory structure

- **app_demo**

     ```
     ├── app        /* Application */
     |    ├── include                           /* Header files, corresponding to the source code files. */
     |    └── src                               /* Source code files */
     |         ├── tuya_ble_sdk_demo.c          /* Entry to the main application. SDK initialization. */
     |         ├── tuya_ble_sdk_test.c          /* Program for testing SDK */
     |         └── tuya_ble_sensor_rht_demo.c   /* Application example */
     |
     ├── board      /* Chipset platform. (Some code is modified on top of the SDK. For more information, see the tutorial.) */
     |    ├── include                           /* Peripheral hardware abstraction interfaces */
     |    |    ├── ty_i2c.h                     /* I2C interface */
     |    |    └── ty_pin.h                     /* PIN interface */
     |    └── TLSR825X
     |         └── ty_board_tlsr825x            /* TLSR825x-specific peripheral driver */
     |              ├── ty_i2c_tlsr825x.c       /* I2C driver */
     |              └── ty_pin_tlsr825x.c       /* PIN driver */
     |
     └── components /* Components */
          └── ty_sht3x                          /* SHT3x driver component */
               ├── ty_sht3x.c                   /* SHT3x driver */
               └── ty_sht3x.h                   /* SHT3x interface */
     ```

- **board_api_demo**

     ```
     ├── app        /* Application */
     |    ├── include                           /* Header files, corresponding to the source code files. */
     |    |    ├── ty_board_demo                /* Board API demo */
     |    |    |   ├── demo_config.h            /* Configuration file for the board API demo, used for demo selection. */
     |    |    |   └── ...
     |    |    └── ...
     |    └── src                               /* Source code files */
     |         ├── ty_board_demo                /* Board API demo */
     |         |    ├── ty_adc_demo.c           /* ADC demo */
     |         |    ├── ty_flash_demo.c         /* FLASH demo */
     |         |    ├── ty_i2c_demo.c           /* I2C demo */
     |         |    ├── ty_pin_demo.c           /* PIN demo */
     |         |    └── ty_pwn_demo.c           /* PWM demo */
     |         ├── tuya_ble_board_api_demo.c    /* Board API example */
     |         ├── tuya_ble_sdk_demo.c          /* Entry to the main application. SDK initialization. */
     |         └── tuya_ble_sdk_test.c          /* Program for testing SDK  */
     |
     └── board      /* Chipset platform. (Some code is modified on top of the SDK. For more information, see the tutorial.) */
          ├── include                           /* Peripheral hardware abstraction interfaces */
          |    ├── ty_adc.h                     /* ADC interface */
          |    ├── ty_i2c.h                     /* I2C interface */
          |    ├── ty_pin.h                     /* PIN interface */
          |    └── ty_pwm.h                     /* PWM interface */
          └── TLSR825X
               └── ty_board_tlsr825x            /* TLSR825x-specific peripheral driver */
                    ├── ty_adc_tlsr825x.c       /* ADC driver */
                    ├── ty_i2c_tlsr825x.c       /* I2C driver */
                    ├── ty_pin_tlsr825x.c       /* PIN driver */
                    └── ty_pwm_tlsr825x.c       /* PWM driver */
     ```

<br>

## Pin configuration

| No. | Symbol | I/O types | Function | Purpose | Remarks |
| :--: | :--: | :------: | :--: | :-----------------: | :------------------: |
| 1 | D3 | I/O | I/O | - | - |
| 2 | D7 | I/O | I/O | PIN Demo - LED output | The LED indicator on the SoC board |
| 3 | C0 | I/O | I/O | Application/I2C Demo - SDA | The SDA on the sensor board |
| 4 | SWS | I/O | SWS | SWS | - |
| 5 | B6 | I | ADC | ADC Demo - ADC | The ADC on the power supply board |
| 6 | A0 | I/O | I/O | PIN Demo - key input | The button on the SoC board |
| 7 | A1 | I/O | I/O | - | - |
| 8 | C2 | I/O | PWM | - | - |
| 9 | C3 | I/O | PWM | Pin for logging output | - |
| 10 | D2 | I/O | PWM | - | - |
| 11 | B4 | I/O | PWM | - | - |
| 12 | B5 | I/O | PWM | PWM Demo - PWM output | PWM5 |
| 13 | GND | P | GND | GND | - |
| 14 | VCC | P | VCC | VCC | - |
| 15 | B1 | I/O | TXD | UART Demo - TX | The default pin. It can be used for flashing firmware. |
| 16 | B7 | I/O | RXD | UART Demo - RX | The default pin. It can be used for flashing firmware. |
| 17 | C4 | I/O | ADC | - | - |
| 18 | RST | I/O | RST | RST | The reset button on the SoC board |
| 19 | C1 | I/O | I/O | Application/I2C Demo - SCL | The SCL on the sensor board |
| 20 | D4 | I/O | I/O | - | - |
| 21 | NC | I/O | No load | - | - |

<br>

## Reference

- [Bluetooth SDK Development](https://developer.tuya.com/en/docs/iot-device-dev/BLE-SDK?id=Kalgco5r2mr0h)
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>

## Technical support

You can get support from Tuya in the following ways:

+ [Tuya Developer](https://developer.tuya.com/en/)
+ [Help Center](https://support.tuya.com/en/help)
+ [Service & Support](https://service.console.tuya.com)
