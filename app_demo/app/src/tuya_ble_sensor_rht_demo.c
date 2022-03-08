/**
 * @file tuya_ble_sensor_rht_demo.c
 * @author lifan
 * @brief tuya sensor/RH-T application based on BLE module
 * @version 1.0
 * @date 2022-02-23
 *
 * @copyright Copyright (c) tuya.inc 2022
 *
 */

#include "tuya_ble_sensor_rht_demo.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_port.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "ty_sht3x.h"
#include "ty_pin.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Pin */
#define LED_PIN                     GPIO_PD7    /* LED PIN */
#define ALERT_PIN                   GPIO_PA1    /* SHT3x ALT PIN */

/* Time */
#define SHT3X_PERI_TIME_MS          1000        /* 1s / 1Hz */
#define LED_TIMER_VAL_MS            300         /* 300ms */

/* Data scale */
#define TEMP_SCALE                  1           /* raw-data * 10 */
#define HUMI_SCALE                  1           /* raw-data * 10 */

/* DP ID */
#define DP_ID_TEMP_CURRENT          1
#define DP_ID_HUMIDITY_VALUE        2
#define DP_ID_MAXTEMP_SET           10
#define DP_ID_MINITEMP_SET          11
#define DP_ID_MAXHUM_SET            12
#define DP_ID_MINIHUM_SET           13
#define DP_ID_TEMP_ALARM            14
#define DP_ID_HUM_ALARM             15

/* DP data index */
#define DP_DATA_INDEX_OFFSET_ID     0
#define DP_DATA_INDEX_OFFSET_TYPE   1
#define DP_DATA_INDEX_OFFSET_LEN_H  2
#define DP_DATA_INDEX_OFFSET_LEN_L  3
#define DP_DATA_INDEX_OFFSET_DATA   4

/* Alarm code */
#define ALARM_LOWER                 0
#define ALARM_UPPER                 1
#define ALARM_CANCEL                2

/* Array size */
#define REPO_ARRAY_SIZE             8

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct {
    int32_t temp;
    int32_t humi;
    uint8_t temp_alarm;
    uint8_t humi_alarm;
} RHT_DP_DATA_T;

typedef struct {
    int32_t temp_max;
    int32_t temp_min;
    int32_t humi_max;
    int32_t humi_min;
} ALARM_THR_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
static RHT_DP_DATA_T sg_rht_data = {
    .temp = 0,
    .humi = 0,
    .temp_alarm = ALARM_CANCEL,
    .humi_alarm = ALARM_CANCEL
};

static ALARM_THR_T sg_alarm_thr = {
    .temp_max = 400,
    .temp_min = 0,
    .humi_max = 600,
    .humi_min = 400
};

static tuya_ble_timer_t sg_rht_daq_timer;
static tuya_ble_timer_t sg_led_flash_timer;

static uint8_t sg_led_status = 0;
static uint8_t sg_repo_array[REPO_ARRAY_SIZE];
static uint32_t sg_sn = 0;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief report one dp data
 * @param[in] dp_id: DP ID
 * @param[in] dp_type: DP type
 * @param[in] dp_len: DP length
 * @param[in] dp_data: DP data address
 * @return none
 */
static void __report_one_dp_data(const uint8_t dp_id, const uint8_t dp_type, const uint16_t dp_len, const uint8_t *dp_data)
{
    uint16_t i;
    sg_repo_array[DP_DATA_INDEX_OFFSET_ID] = dp_id;
    sg_repo_array[DP_DATA_INDEX_OFFSET_TYPE] = dp_type;
    sg_repo_array[DP_DATA_INDEX_OFFSET_LEN_H] = (uint8_t)(dp_len >> 8);
    sg_repo_array[DP_DATA_INDEX_OFFSET_LEN_L] = (uint8_t)dp_len;
    for (i = 0; i < dp_len; i++) {
        sg_repo_array[DP_DATA_INDEX_OFFSET_DATA + i] = *(dp_data + (dp_len-i-1));
    }
    tuya_ble_dp_data_send(sg_sn++, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, sg_repo_array, dp_len + DP_DATA_INDEX_OFFSET_DATA);
}

/**
 * @brief report all DP data
 * @param none
 * @return none
 */
static void __repo_dp_data_all(void)
{
    static RHT_DP_DATA_T s_prv_val = {
        .temp = 0,
        .humi = 0,
        .temp_alarm = ALARM_CANCEL,
        .humi_alarm = ALARM_CANCEL
    };

    if (sg_rht_data.temp != s_prv_val.temp) {
        __report_one_dp_data(DP_ID_TEMP_CURRENT, DT_VALUE, 4, (uint8_t *)&sg_rht_data.temp);
        s_prv_val.temp = sg_rht_data.temp;
    }
    if (sg_rht_data.humi != s_prv_val.humi) {
        __report_one_dp_data(DP_ID_HUMIDITY_VALUE, DT_VALUE, 4, (uint8_t *)&sg_rht_data.humi);
        s_prv_val.humi = sg_rht_data.humi;
    }
    if (sg_rht_data.temp_alarm != s_prv_val.temp_alarm) {
        __report_one_dp_data(DP_ID_TEMP_ALARM, DT_ENUM, 1, (uint8_t *)&sg_rht_data.temp_alarm);
        TUYA_APP_LOG_DEBUG("report dp_temp_alarm: %d.", sg_rht_data.temp_alarm);
        s_prv_val.temp_alarm = sg_rht_data.temp_alarm;
    }
    if (sg_rht_data.humi_alarm != s_prv_val.humi_alarm) {
        __report_one_dp_data(DP_ID_HUM_ALARM, DT_ENUM, 1, (uint8_t *)&sg_rht_data.humi_alarm);
        TUYA_APP_LOG_DEBUG("report dp_humi_alarm: %d.", sg_rht_data.humi_alarm);
        s_prv_val.humi_alarm = sg_rht_data.humi_alarm;
    }
}

/**
 * @brief led flash timer callback
 * @param none
 * @return none
 */
static void __led_flash_timer_cb(void)
{
    sg_led_status = !sg_led_status;
    if (sg_led_status) {
        ty_pin_set(LED_PIN, TY_PIN_HIGH);
    } else {
        ty_pin_set(LED_PIN, TY_PIN_LOW);
    }
}

/**
 * @brief network process init
 * @param none
 * @return none
 */
static void __net_proc_init(void)
{
    /* led init */
    ty_pin_init(LED_PIN, TY_PIN_MODE_OUT_PP_LOW);
    tuya_ble_timer_create(&sg_led_flash_timer, LED_TIMER_VAL_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__led_flash_timer_cb);
    /* check ble connect status */
    tuya_ble_connect_status_t ble_conn_sta = tuya_ble_connect_status_get();
    TUYA_APP_LOG_DEBUG("BLE connect status: %d.", ble_conn_sta);
    if ((ble_conn_sta == BONDING_UNCONN) ||
        (ble_conn_sta == BONDING_CONN)   ||
        (ble_conn_sta == BONDING_UNAUTH_CONN)) {
        TUYA_APP_LOG_INFO("LED keep off.");
    } else {
        sg_led_status = 1;
        ty_pin_set(LED_PIN, TY_PIN_HIGH);
        tuya_ble_timer_start(sg_led_flash_timer);
        TUYA_APP_LOG_INFO("LED start falshing.");
    }
}

/**
 * @brief ble connected process
 * @param none
 * @return none
 */
void tuya_net_proc_ble_conn(void)
{
    sg_led_status = 0;
    ty_pin_set(LED_PIN, TY_PIN_LOW);
    tuya_ble_timer_stop(sg_led_flash_timer);
    TUYA_APP_LOG_INFO("LED stop falshing.");
}

/**
 * @brief ble unbound process
 * @param none
 * @return none
 */
void tuya_net_proc_ble_unbound(void)
{
    sg_led_status = 1;
    ty_pin_set(LED_PIN, TY_PIN_HIGH);
    tuya_ble_timer_start(sg_led_flash_timer);
    TUYA_APP_LOG_INFO("LED start falshing.");
}

/**
 * @brief received DP data process
 * @param[in] dp_data: dp data array
 * @param[in] dp_len: dp data length
 * @return none
 */
void tuya_net_proc_dp_recv(uint8_t *dp_data, const uint16_t dp_len)
{
	int32_t val = 0;
    if (dp_len - 4 == 4) {
        val = dp_data[4] << 24 | dp_data[5] << 16 | dp_data[6] << 8 | dp_data[7];
        TUYA_APP_LOG_DEBUG("val: %x", val);
    }
    switch (dp_data[0]) {
    case DP_ID_MAXTEMP_SET:
        sg_alarm_thr.temp_max = val;
        __report_one_dp_data(DP_ID_MAXTEMP_SET, DT_VALUE, 4, (uint8_t *)&val);
        TUYA_APP_LOG_INFO("Set the maximum temperature to %d.", sg_alarm_thr.temp_max);
        break;
    case DP_ID_MINITEMP_SET:
        sg_alarm_thr.temp_min = val;
        __report_one_dp_data(DP_ID_MINITEMP_SET, DT_VALUE, 4, (uint8_t *)&val);
        TUYA_APP_LOG_INFO("Set the minimum temperature to %d.", sg_alarm_thr.temp_min);
        break;
    case DP_ID_MAXHUM_SET:
        sg_alarm_thr.humi_max = val;
        __report_one_dp_data(DP_ID_MAXHUM_SET, DT_VALUE, 4, (uint8_t *)&val);
        TUYA_APP_LOG_INFO("Set the maximum humidity to %d.", sg_alarm_thr.humi_max);
        break;
    case DP_ID_MINIHUM_SET:
        sg_alarm_thr.humi_min = val;
        __report_one_dp_data(DP_ID_MINIHUM_SET, DT_VALUE, 4, (uint8_t *)&val);
        TUYA_APP_LOG_INFO("Set the minimum humidity to %d.", sg_alarm_thr.humi_min);
        break;
    default:
        break;
    }
}

/**
 * @brief check temperature value
 * @param[in] cur_temp: current temperature
 * @return alarm code
 */
static uint8_t __check_temp_val(const int32_t cur_temp)
{
    uint8_t res;
    if (cur_temp < sg_alarm_thr.temp_min) {
        res = ALARM_LOWER;
    } else if (cur_temp > sg_alarm_thr.temp_max) {
        res = ALARM_UPPER;
    } else {
        res = ALARM_CANCEL;
    }
    return res;
}

/**
 * @brief check humidity value
 * @param[in] cur_humi: current humidity
 * @return alarm code
 */
static uint8_t __check_humi_val(const int32_t cur_humi)
{
    uint8_t res;
    if (cur_humi < sg_alarm_thr.humi_min) {
        res = ALARM_LOWER;
    } else if (cur_humi > sg_alarm_thr.humi_max) {
        res = ALARM_UPPER;
    } else {
        res = ALARM_CANCEL;
    }
    return res;
}

/**
 * @brief RH-T sensor data acquisition timer handler
 * @param none
 * @return none
 */
static void __rht_daq_timer_handler(void)
{
    int32_t temp, humi;
    if (ty_sht3x_read_data(&temp, &humi, TEMP_SCALE, HUMI_SCALE)) {
        TUYA_APP_LOG_DEBUG("Temperature: %d, Humidity: %d", temp, humi);
        sg_rht_data.temp = temp;
        sg_rht_data.humi = humi;
        sg_rht_data.temp_alarm = __check_temp_val(temp);
        sg_rht_data.humi_alarm = __check_humi_val(humi);
        if (BONDING_CONN == tuya_ble_connect_status_get()) {
            __repo_dp_data_all();
        }
    } else {
        TUYA_APP_LOG_ERROR("ty_sht3x_read_data failed.");
    }
}

/**
 * @brief RH-T sensor application init
 * @param none
 * @return none
 */
void tuya_ble_sensor_rht_init(void)
{
    TUYA_APP_LOG_INFO("Sensor RH-T demo start.");

    /* network process init */
    __net_proc_init();

    /* SHT3x init */
    ty_sht3x_init(0);

    /* measure once */
    if (ty_sht3x_measure_single_shot(&sg_rht_data.temp, &sg_rht_data.humi, TEMP_SCALE, HUMI_SCALE)) {
        TUYA_APP_LOG_DEBUG("Temperature: %d, Humidity: %d", sg_rht_data.temp, sg_rht_data.humi);
    } else {
        TUYA_APP_LOG_ERROR("ty_sht3x_measure_single_shot failed.");
    }

    /* start periodic measurement */
    ty_sht3x_start_periodic_measure(REPEATAB_HIGH, FREQ_1HZ);
    tuya_ble_timer_create(&sg_rht_daq_timer, SHT3X_PERI_TIME_MS, TUYA_BLE_TIMER_REPEATED, (tuya_ble_timer_handler_t)__rht_daq_timer_handler);
    tuya_ble_timer_start(sg_rht_daq_timer);
}
