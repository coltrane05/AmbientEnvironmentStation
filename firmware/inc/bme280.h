#ifndef BME280_H
#define BME280_H

#include <stdint.h>

#define BME280_ADDR 0x76 
#define BME280_TEMP_CAL_START_REG 0x88
#define BME280_PRES_CAL_START_REG 0x8E
#define BME280_HUM_CAL_START_REG1 0xA1
#define BME280_HUM_CAL_START_REG2 0xE1

#define BME280_TEMP_DATA_START_REG 0xFA


void get_bme_cal_data(void);
void bme280_init(void);
int32_t get_bme_temp_data(void);
void format_temp_string(int32_t temp, char* buffer, uint32_t max_len);
int32_t calculate_bme_temp(void);
int32_t convert_celsius_to_fahrenheit(int32_t temp_c);

#endif