#ifndef BME280_H
#define BME280_H

#include <stdint.h>

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

void reset_check_BME(void);
void set_check_BME(void);
uint8_t get_check_BME(void);

void get_bme_cal_data(void);

void bme280_init(void);

void get_bme_data(char** string_buffers, uint32_t max_len);

void format_temp_string(int32_t temp, char* buffer, uint32_t max_len);
void format_pres_string(uint32_t pres, char* buffer, uint32_t max_len);
void format_hum_string(uint32_t hum, char* buffer, uint32_t max_len);

int32_t calculate_bme_temp(int32_t adc_T);
uint32_t calculate_bme_pres(int32_t adc_P);
uint32_t calculate_bme_hum(int32_t adc_H);

int32_t convert_celsius_to_fahrenheit(int32_t temp_c);

#endif