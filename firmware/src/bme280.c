#include "bme280.h"
#include "i2c.h"

static uint32_t dig_T1 = 0;
static int32_t dig_T2 = 0;
static int32_t dig_T3 = 0;

// static uint8_t temp_cal_buffer[6];
// static uint8_t temp_data_buffer[3];

static int32_t t_fine;

// function to get the calibration data from the BME280
void get_bme_cal_data(void) {
    uint8_t temp_cal_buffer[6]; // Temperature calibration data is 6 bytes
    I2C1_master_receive(BME280_ADDR, BME280_TEMP_CAL_START_REG, 6, temp_cal_buffer);
    dig_T1 = (temp_cal_buffer[1] << 8) | temp_cal_buffer[0]; // BME structures their calibration data little endian
    dig_T2 = (temp_cal_buffer[3] << 8) | temp_cal_buffer[2];
    dig_T3 = (temp_cal_buffer[5] << 8) | temp_cal_buffer[4];
}

// Initialize the BME280 sensor and get calibration data
void bme280_init(void) {
    I2C1_master_write_register(BME280_ADDR, 0xF4, 0x23); // Write to register F4 to set up sensor

    get_bme_cal_data(); // Get calibration data and store in static global variables
}

// Get the temperature data from the BME280 and stort it in a signed 32-bit integer
int32_t get_bme_temp_data(void) {
    uint8_t temp_data_buffer[3]; // Temperature data is stored accross 3 bytes
    I2C1_master_receive(BME280_ADDR, BME280_TEMP_DATA_START_REG, 3, temp_data_buffer);
    int32_t adc_T = ((uint32_t)temp_data_buffer[0] << 12) | 
                    ((uint32_t)temp_data_buffer[1] << 4)  | 
                    (temp_data_buffer[2] >> 4);

    return adc_T;
}

// Take the temp integer and format it into a string for display
void format_temp_string(int32_t temp, char* buffer, uint32_t max_len) {
    if (max_len == 0 || buffer == 0) return;

    uint8_t is_negative = 0;

    if (temp < 0) {
        is_negative = 1;
        temp = -temp;
    }

    int32_t whole = temp / 100;
    int32_t decimal = temp % 100;

    char reversed_whole[10];
    int i = 0;

    if (whole == 0) {
        reversed_whole[i++] = '0';
    }
    else {
        while (whole > 0 && i < 10) {
            reversed_whole[i++] = (whole % 10) + '0';
            whole /= 10;
        }
    }

    uint32_t buff_idx = 0;
    
    if (is_negative && buff_idx < max_len - 1) {
        buffer[buff_idx++] = '-';
    }

    while (i > 0 && buff_idx < max_len - 1) {
        buffer[buff_idx++] = reversed_whole[--i];
    }

    if (buff_idx < max_len - 1) buffer[buff_idx++] = '.';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal / 10) + '0';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal % 10) + '0';
    
    buffer[buff_idx] = '\0';
}   

// Use the calibration data to calculate temperature in celsius.
int32_t calculate_bme_temp(void) {
    
    int32_t adc_T = get_bme_temp_data();

    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Convert celsius temp to fahrenheit
int32_t convert_celsius_to_fahrenheit(int32_t temp_c) {

    if(temp_c >= 0) {
        return (((temp_c * 9) + 2) / 5) + 3200;
    }
    else {
        return (((temp_c * 9) - 2) / 5) + 3200;
    }
}
