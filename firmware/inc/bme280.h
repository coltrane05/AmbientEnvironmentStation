#ifndef BME280_H
#define BME280_H

#include <stdint.h>
#include <stdbool.h>

#define BME280_ADDR 0x76 

#define BME280_CTRL_MEAS_REG 0xF4
#define BME280_CTRL_HUM_REG 0xF2 

#define BME280_CTRL_MEAS_VAL 0x27
#define BME280_CTRL_HUM_VAL 0x01

#define BME280_TEMP_CAL_START_REG 0x88
#define BME280_PRES_CAL_START_REG 0x8E
#define BME280_HUM_CAL_START_REG1 0xA1
#define BME280_HUM_CAL_START_REG2 0xE1

#define BME280_DATA_START_REG 0xF7
#define BME280_TEMP_DATA_START_REG 0xFA
#define BME280_PRES_DATA_START_REG 0xF7
#define BME280_HUM_DATA_START_REG 0xFD

typedef enum
{
    BME280_TEMP,
    BME280_PRES,
    BME280_HUM,
    BME280_ALL
} bme280_display_state_t;

void reset_check_BME (void);
void set_check_BME (void);
uint8_t get_check_BME (void);

void get_bme_cal_data (void);

void bme280_init (void);

void start_bme_data_collection (void);

bool bme_data_is_ready (void);
void process_and_print_bme_data (void);
void process_and_display_all_bme_data (void);
void process_and_display_bme_temperature_data (void);
void process_and_display_bme_pressure_data (void);
void process_and_display_bme_humidity_data (void);

void bme280_increment_display_state (void);
void bme280_decrement_display_state (void);
bme280_display_state_t get_bme280_display_state (void);

void toggle_celsius_mode (void);
bool is_in_celsius_mode (void);

void set_celsius_mode_changed (void);
void reset_celsius_mode_changed (void);
bool celsius_mode_changed (void);

#endif