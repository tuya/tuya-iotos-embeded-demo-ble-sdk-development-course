# Tuya BLE Module SDK Development Course

<br>

## 简介

本 Demo 为 [蓝牙 LE SDK 开发教程](https://developer.tuya.com/cn/docs/iot/bluetooth-le-sdk-development?id=Kbe6do0lc2wc2) 的配套 Demo。

- `app_demo`：[第 5 课：应用开发](https://developer.tuya.com/cn/docs/iot/application-development?id=Kbe6embsa0wtu) 配套 Demo，演示了使用蓝牙模组 SDK 开发一款温湿度传感产品的方法。
- `board_api_demo`：[第 8 课：Board API 说明]() 配套 Demo，演示了 Board 层 API 的使用方法。

<br>

## 开发环境

本 Demo 的开发环境如下：

- 涂鸦三明治 BLE SoC 主控板 (BTU)

- 涂鸦三明治 温湿度传感器 功能版 (SHT30-DIS)

- USB 转串口工具、串口调试助手

- 生产解决方案 (涂鸦云模组烧录授权平台)

- Telink 烧录器、Telink IDE、Telink BDT

- PC (Windows 10)

- SDK (tuya-ble-sdk-demo-project-tlsr8253)

- 智能手机、智能生活 APP

<br>

## 文件说明

- **app_demo**

```
├── app        /* 应用 */
|    ├── include                           /* 头文件目录，与源文件一一对应 */
|    └── src                               /* 源文件目录 */
|         ├── tuya_ble_sdk_demo.c          /* 实现tuya_ble_sdk的初始化，应用程序入口 */
|         ├── tuya_ble_sdk_test.c          /* SDK测试程序 */
|         └── tuya_ble_sensor_rht_demo.c   /* 温湿度传感应用示例程序 */
|
├── board      /* 芯片平台 (SDK 基础上修改了部分代码，具体说明请参考配套教程) */
|    ├── include                           /* 外设硬件抽象统一接口 */
|    |    ├── ty_i2c.h                     /* I2C 接口 */
|    |    └── ty_pin.h                     /* PIN 接口 */
|    └── TLSR825X
|         └── ty_board_tlsr825x            /* TLSR825x 芯片平台对应的外设驱动代码 */
|              ├── ty_i2c_tlsr825x.c       /* I2C 驱动 */
|              └── ty_pin_tlsr825x.c       /* PIN 驱动 */
|
└── components /* 组件 */
     └── ty_sht3x                          /* SHT3x 驱动组件 */
          ├── ty_sht3x.c                   /* SHT3x 驱动 */
          └── ty_sht3x.h                   /* SHT3x 接口 */
```

- **board_api_demo**

```
├── app        /* 应用 */
|    ├── include                           /* 头文件目录，省略部分与源文件一一对应 */
|    |    ├── ty_board_demo                /* Board API demo */
|    |    |   ├── demo_config.h            /* Board API demo 配置文件，用于 demo 选择 */
|    |    |   └── ...
|    |    └── ...
|    └── src                               /* 源文件目录 */
|         ├── ty_board_demo                /* Board API demo */
|         |    ├── ty_adc_demo.c           /* ADC demo */
|         |    ├── ty_flash_demo.c         /* FLASH demo */
|         |    ├── ty_i2c_demo.c           /* I2C demo */
|         |    ├── ty_pin_demo.c           /* PIN demo */
|         |    └── ty_pwn_demo.c           /* PWM demo */
|         ├── tuya_ble_board_api_demo.c    /* Board API 示例程序 */
|         ├── tuya_ble_sdk_demo.c          /* 实现tuya_ble_sdk的初始化，应用程序入口 */
|         └── tuya_ble_sdk_test.c          /* SDK测试程序 */
|
└── board      /* 芯片平台 (SDK 基础上修改了部分代码，具体说明请参考配套教程) */
     ├── include                           /* 外设硬件抽象统一接口 */
     |    ├── ty_adc.h                     /* ADC 接口 */
     |    ├── ty_i2c.h                     /* I2C 接口 */
     |    ├── ty_pin.h                     /* PIN 接口 */
     |    └── ty_pwm.h                     /* PWM 接口 */
     └── TLSR825X
          └── ty_board_tlsr825x            /* TLSR825x 芯片平台对应的外设驱动 */
               ├── ty_adc_tlsr825x.c       /* ADC 驱动 */
               ├── ty_i2c_tlsr825x.c       /* I2C 驱动 */
               ├── ty_pin_tlsr825x.c       /* PIN 驱动 */
               └── ty_pwm_tlsr825x.c       /* PWM 驱动 */
```

<br>

## I/O 列表

| 序号 | 符号 | I/O 类型 | 功能 |        用途         |         备注         |
| :--: | :--: | :------: | :--: | :-----------------: | :------------------: |
|  1   |  D3  |   I/O    | I/O  |          -          |          -           |
|  2   |  D7  |   I/O    | I/O  | PIN Demo - LED 输出 |    主控板的指示灯    |
|  3   |  C0  |   I/O    | I/O  | 应用/I2C Demo - SDA |   对应功能板的 SDA   |
|  4   | SWS  |   I/O    | SWS  |         SWS         |          -           |
|  5   |  B6  |    I     | ADC  |   ADC Demo - ADC    |   对应电源板的 ADC   |
|  6   |  A0  |   I/O    | I/O  | PIN Demo - 按键输入 |     主控板的按键     |
|  7   |  A1  |   I/O    | I/O  |          -          |          -           |
|  8   |  C2  |   I/O    | PWM  |          -          |          -           |
|  9   |  C3  |   I/O    | PWM  |      LOG 引脚       |          -           |
|  10  |  D2  |   I/O    | PWM  |          -          |          -           |
|  11  |  B4  |   I/O    | PWM  |          -          |          -           |
|  12  |  B5  |   I/O    | PWM  | PWM Demo - PWM 输出 |         PWM5         |
|  13  | GND  |    P     | GND  |         GND         |          -           |
|  14  | VCC  |    P     | VCC  |         VCC         |          -           |
|  15  |  B1  |   I/O    | TXD  |   UART Demo - TX    | SDK 默认，可用于烧录 |
|  16  |  B7  |   I/O    | RXD  |   UART Demo - RX    | SDK 默认，可用于烧录 |
|  17  |  C4  |   I/O    | ADC  |          -          |          -           |
|  18  | RST  |   I/O    | RST  |         RST         |    主控板的复位键    |
|  19  |  C1  |   I/O    | I/O  | 应用/I2C Demo - SCL |   对应功能板的 SCL   |
|  20  |  D4  |   I/O    | I/O  |          -          |          -           |
|  21  |  NC  |   I/O    | 空接 |          -          |          -           |

<br>

## 相关文档

- [蓝牙 SDK 开发](https://developer.tuya.com/cn/docs/iot-device-dev/BLE-SDK?id=Kalgco5r2mr0h)
- [涂鸦 Demo 中心](https://developer.tuya.com/demo)

<br>

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- [涂鸦 AI+IoT 开发者平台](https://developer.tuya.com)
- [帮助中心](https://support.tuya.com/help)
- [服务与支持](https://service.console.tuya.com)

