#include "bme280.h"
#include "i2c.h"
#include "usart.h"
#include "ili9341.h"
#include "spi.h"
#include "neopixel_ring.h"
#include <stddef.h>

// Calibration data for temperature and pressure and humidity.
// These values are read from the BME280 sensor during initialization
// and used to calculate sensor values from the raw analogue data sent
// by the sensor.
static uint16_t dig_T1 = 0;
static int16_t dig_T2 = 0;
static int16_t dig_T3 = 0;

static uint16_t dig_P1 = 0;
static int16_t dig_P2 = 0;
static int16_t dig_P3 = 0;
static int16_t dig_P4 = 0;
static int16_t dig_P5 = 0;
static int16_t dig_P6 = 0;
static int16_t dig_P7 = 0;
static int16_t dig_P8 = 0;
static int16_t dig_P9 = 0;

static uint8_t dig_H1 = 0;
static int16_t dig_H2 = 0;
static uint8_t dig_H3 = 0;
static int16_t dig_H4 = 0;
static int16_t dig_H5 = 0;
static int8_t dig_H6 = 0;

static int32_t adc_P;
static int32_t adc_T;
static int32_t adc_H;

static int32_t T;
static uint32_t P;
static uint32_t H;

static volatile uint8_t check_BME = 0;

static uint8_t background_data_buffer[8];
volatile bool bme_data_ready = false;

static bool celsius_mode = false;
static bool celsius_mode_changed_flag = false; 

static bme280_display_state_t display_state = 0;

static int32_t t_fine;

// Private fuction declarations
void process_data_helper (void);

void format_temp_string (int32_t temp, char* buffer, uint32_t max_len);
void format_pres_string (uint32_t pres, char* buffer, uint32_t max_len);
void format_hum_string (uint32_t hum, char* buffer, uint32_t max_len);

int32_t calculate_bme_temp (int32_t adc_T);
uint32_t calculate_bme_pres (int32_t adc_P);
uint32_t calculate_bme_hum (int32_t adc_H);

int32_t convert_celsius_to_fahrenheit(int32_t temp_c);

void reset_check_BME(void) 
{
    check_BME = 0;
}

void set_check_BME(void) 
{
    check_BME = 1;
}

uint8_t get_check_BME(void) 
{
    return check_BME;
}

// function to get the calibration data from the BME280
void get_bme_cal_data(void) 
{
    uint8_t temp_cal_buffer[6]; // Temperature calibration data is 6 bytes
    uint8_t pres_cal_buffer[18]; // Pressure calibration data is 18 bytes
    uint8_t hum_cal_buffer_1[1]; // Humidity calibration data part 1 is 1 byte
    uint8_t hum_cal_buffer_2[7]; // Humidity calibration data part 2 is 7 bytes

    I2C1_master_receive(BME280_ADDR, BME280_TEMP_CAL_START_REG, 6, temp_cal_buffer, NULL);
    while(I2C_read_is_busy());
    I2C1_master_receive(BME280_ADDR, BME280_PRES_CAL_START_REG, 18, pres_cal_buffer, NULL);
    while(I2C_read_is_busy());
    I2C1_master_receive(BME280_ADDR, BME280_HUM_CAL_START_REG1, 1, hum_cal_buffer_1, NULL);
    while(I2C_read_is_busy());
    I2C1_master_receive(BME280_ADDR, BME280_HUM_CAL_START_REG2, 7, hum_cal_buffer_2, NULL);
    while(I2C_read_is_busy());

    dig_T1 = (uint16_t)((temp_cal_buffer[1] << 8) | temp_cal_buffer[0]); // BME structures their calibration data little endian
    dig_T2 = (int16_t)((temp_cal_buffer[3] << 8) | temp_cal_buffer[2]);
    dig_T3 = (int16_t)((temp_cal_buffer[5] << 8) | temp_cal_buffer[4]);

    dig_P1 = (uint16_t)((pres_cal_buffer[1] << 8) | pres_cal_buffer[0]);
    dig_P2 = (int16_t)((pres_cal_buffer[3] << 8) | pres_cal_buffer[2]);
    dig_P3 = (int16_t)((pres_cal_buffer[5] << 8) | pres_cal_buffer[4]);
    dig_P4 = (int16_t)((pres_cal_buffer[7] << 8) | pres_cal_buffer[6]);
    dig_P5 = (int16_t)((pres_cal_buffer[9] << 8) | pres_cal_buffer[8]);
    dig_P6 = (int16_t)((pres_cal_buffer[11] << 8) | pres_cal_buffer[10]);
    dig_P7 = (int16_t)((pres_cal_buffer[13] << 8) | pres_cal_buffer[12]);
    dig_P8 = (int16_t)((pres_cal_buffer[15] << 8) | pres_cal_buffer[14]);
    dig_P9 = (int16_t)((pres_cal_buffer[17] << 8) | pres_cal_buffer[16]);

    dig_H1 = hum_cal_buffer_1[0];
    dig_H2 = (int16_t)((hum_cal_buffer_2[1] << 8) | hum_cal_buffer_2[0]);
    dig_H3 = hum_cal_buffer_2[2];
    dig_H4 = (int16_t)(((int8_t)hum_cal_buffer_2[3] << 4) | (hum_cal_buffer_2[4] & 0x0F));
    dig_H5 = (int16_t)(((int8_t)hum_cal_buffer_2[5] << 4) | hum_cal_buffer_2[4] >> 4);
    dig_H6 = (int8_t)hum_cal_buffer_2[6];
}

// Initialize the BME280 sensor and get calibration data
void bme280_init(void) 
{
    I2C1_master_write_register(BME280_ADDR, BME280_CTRL_HUM_REG, BME280_CTRL_HUM_VAL); // setup humidity
    I2C1_master_write_register(BME280_ADDR, BME280_CTRL_MEAS_REG, BME280_CTRL_MEAS_VAL); // setup pressure and temperature
    
    get_bme_cal_data(); // Get calibration data and store in static global variables
}

// Use the calibration data to calculate temperature in celsius.
// This function takes the adc_T value as input and returns
// the temperature as a signed 32-bit integer.
// The formula was pulled from the BME280 datasheet from bosch.
int32_t calculate_bme_temp(int32_t adc_T) 
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T;
}

// Use the calibration data to calculate pressure in pascals.
// This function takes the adc_P value as input and returns
// the barometric pressure as an unsigned 32-bit integer.
// The formula was pulled form the BME280 datasheet from bosch.
uint32_t calculate_bme_pres(int32_t adc_P) 
{

    int32_t var1, var2;
    uint32_t p;
    var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
    var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((int32_t)dig_P1))>>15);

    if (var1 == 0) 
    {
        return 0;
    }

    p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;

    if (p < 0x80000000) 
    {
        p = (p << 1) / ((uint32_t)var1);
    } else 
    {
        p = (p / (uint32_t)var1) * 2;
    }

    var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;

    p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));

    return (uint32_t)p;
}

// Use the calibration data to calculate percent humidity.
// This function takes the adc_H value as input and returns
// the relative humidity as an unsigned 32-bit integer.
// The formula was pulled form the BME280 datasheet from bosch.
uint32_t calculate_bme_hum(int32_t adc_H) 
{

    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) *
        v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
        ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
        ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) +
        8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
        ((int32_t)dig_H1)) >> 4));
    
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (uint32_t)(v_x1_u32r >> 12);
}

// Convert celsius temp to fahrenheit
int32_t convert_celsius_to_fahrenheit(int32_t temp_c) 
{

    if(temp_c >= 0) 
    {
        return (((temp_c * 9) + 2) / 5) + 3200;
    }
    else 
    {
        return (((temp_c * 9) - 2) / 5) + 3200;
    }
}

// Function to format temperature value into a string that can be displayed over
// UART terminal or LCD.
void format_temp_string(int32_t temp, char* buffer, uint32_t max_len) 
{
    if (max_len == 0 || buffer == 0) return;

    uint8_t is_negative = 0;

    if (temp < 0) 
    {
        is_negative = 1;
        temp = -temp;
    }

    int32_t whole = temp / 100;
    int32_t decimal = temp % 100;

    char reversed_whole[10];
    int i = 0;

    if (whole == 0) 
    {
        reversed_whole[i++] = '0';
    }
    else 
    {
        while (whole > 0 && i < 10) 
        {
            reversed_whole[i++] = (whole % 10) + '0';
            whole /= 10;
        }
    }

    uint32_t buff_idx = 0;
    
    if (is_negative && buff_idx < max_len - 1) 
    {
        buffer[buff_idx++] = '-';
    }

    while (i > 0 && buff_idx < max_len - 1) 
    {
        buffer[buff_idx++] = reversed_whole[--i];
    }

    if (buff_idx < max_len - 1) buffer[buff_idx++] = '.';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal / 10) + '0';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal % 10) + '0';

    if (buff_idx < max_len - 1) buffer[buff_idx++] = ' ';

    if (celsius_mode)
    {
        buffer[buff_idx++] = 'C';
    }
    else
    {
        buffer[buff_idx++] = 'F';
    }
    
    buffer[buff_idx] = '\0';
}

// Function to format pressure value into a string that can be displayed over
// UART terminal or LCD.
void format_pres_string(uint32_t pres, char* buffer, uint32_t max_len) 
{
    if (max_len == 0 || buffer == 0) return;

    uint32_t whole = pres / 100; // Convert Pa to hPa for the whole number
    uint32_t decimal = pres % 100; // Extract remainder for the decimal part

    char reversed_whole[10];
    int i = 0;

    if (whole == 0) 
    {
        reversed_whole[i++] = '0';
    }
    else 
    {
        while (whole > 0 && i < 10) 
        {
            reversed_whole[i++] = (whole % 10) + '0';
            whole /= 10;
        }
    }

    uint32_t buff_idx = 0;
    while (i > 0 && buff_idx < max_len - 1) 
    {
        buffer[buff_idx++] = reversed_whole[--i];
    }

    if (buff_idx < max_len - 1) buffer[buff_idx++] = '.';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal / 10) + '0';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (decimal % 10) + '0';

    if (buff_idx < max_len - 1) buffer[buff_idx++] = ' ';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = 'h';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = 'P';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = 'a';

    buffer[buff_idx] = '\0';
}

// Function to format humidity value into a string that can be displayed over
// UART terminal or LCD.
void format_hum_string(uint32_t hum, char* buffer, uint32_t max_len) 
{
    if (max_len == 0 || buffer == 0) return;

    uint32_t whole = hum / 1024;
    uint32_t decimal = hum % 1024;

    char reversed_whole[10];
    int i = 0;

    if (whole == 0) 
    {
        reversed_whole[i++] = '0';
    }
    else 
    {
        while (whole > 0 && i < 10) 
        {
            reversed_whole[i++] = (whole % 10) + '0';
            whole /= 10;
        }
    }

    uint32_t buff_idx = 0;
    while (i > 0 && buff_idx < max_len - 1) 
    {
        buffer[buff_idx++] = reversed_whole[--i];
    }

    if (buff_idx < max_len - 1) buffer[buff_idx++] = '.';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (((decimal * 1000) / 1024) / 100) + '0';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = ((((decimal * 1000) / 1024) % 100) / 10) + '0';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = (((decimal * 1000) / 1024) % 10) + '0';

    if (buff_idx < max_len - 1) buffer[buff_idx++] = ' ';
    if (buff_idx < max_len - 1) buffer[buff_idx++] = '%';

    buffer[buff_idx] = '\0';
}

// Callback function that gets called when the I2C state machine has finished
// reading data from the BME280
void bme_data_ready_callback(void) 
{
    bme_data_ready = true;
}

// Function to get the state of BME280 data. Used in the main function to check
// if data is ready for processing.
bool bme_data_is_ready(void) 
{
    return bme_data_ready;
}

void process_data_helper(void)
{
    adc_P = ((uint32_t)background_data_buffer[0] << 12) |
                    ((uint32_t)background_data_buffer[1] << 4)  |
                    (background_data_buffer[2] >> 4);

    adc_T = ((uint32_t)background_data_buffer[3] << 12) |
                    ((uint32_t)background_data_buffer[4] << 4)  |
                    (background_data_buffer[5] >> 4);

    adc_H = ((uint32_t)background_data_buffer[6] << 8) |
                    ((uint32_t)background_data_buffer[7]);

    T = calculate_bme_temp(adc_T);
    P = calculate_bme_pres(adc_P);
    H = calculate_bme_hum(adc_H);
}

// Function to take raw all raw analog data from the BME280 and convert it to
// human readable data and print it to the USART terminal.
void process_and_print_bme_data(void) 
{
    process_data_helper();

    char pres_buffer[15];
    char temp_buffer[15];
    char hum_buffer[15];

    uint32_t max_len = 15;

    format_pres_string(P, pres_buffer, max_len);
    format_temp_string(T, temp_buffer, max_len);
    format_hum_string(H, hum_buffer, max_len);

    usart2_print("Pressure: ");
    usart2_print(pres_buffer);
    usart2_println(" hPa");

    usart2_print("Temperature: ");
    usart2_print(temp_buffer);
    usart2_println(" F");
    
    usart2_print("Humidity: ");
    usart2_print(hum_buffer);
    usart2_println("%");
    usart2_println("");
    bme_data_ready = false;
}

void process_and_display_all_bme_data(void)
{
    process_data_helper();

    char pres_buffer[15];
    char temp_buffer[15];
    char hum_buffer[15];

    uint32_t max_len = 15;

    if (!celsius_mode)
    {
        T = convert_celsius_to_fahrenheit(T);
    }

    format_pres_string(P, pres_buffer, max_len);
    format_temp_string(T, temp_buffer, max_len);
    format_hum_string(H, hum_buffer, max_len);

    if (text_bounds.text_is_displayed)
    {
        ili9341_clear_text();
        // Wait for the clear-text DMA to finish before starting the draw
        while (screen_draw_is_busy())
        {
            spi2_process_callbacks();
        }
    }

    ili9341_draw_string(temp_buffer, 150, 100, 0xFFFF, 0x0000);
    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    ili9341_draw_string(pres_buffer, 150, 130, 0xFFFF, 0x0000);
    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    ili9341_draw_string(hum_buffer, 150, 160, 0xFFFF, 0x0000);
    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    bme_data_ready = false;
}

void process_and_display_bme_temperature_data(void)
{
    process_data_helper();

    char temp_buffer[15];
    uint32_t max_len = 15;

    neopixel_color_t temp_color;
    
    get_temperature_color(T, 1555, 2600, &temp_color.r, &temp_color.g, &temp_color.b);
    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        set_neopixel_color(temp_color, i);
    }

    if (!celsius_mode)
    {
        T = convert_celsius_to_fahrenheit(T);
    }

    format_temp_string(T, temp_buffer, max_len);

    // Only clear previous text if there was actually text displayed
    if (text_bounds.text_is_displayed)
    {
        ili9341_clear_text();
        // Wait for the clear-text DMA to finish before starting the draw
        while (screen_draw_is_busy())
        {
            spi2_process_callbacks();
        }
    }

    ili9341_draw_string(temp_buffer, 150, 120, 0xFFFF, 0x0000);

    // Wait for the screen to finish drawing before returning to the main loop,
    // so that screen_draw_busy is false and the next BME update can proceed.
    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    send_neopixel_data();

    bme_data_ready = false;
}

void process_and_display_bme_pressure_data(void)
{
    process_data_helper();

    char pres_buffer[15];
    uint32_t max_len = 15;

    neopixel_color_t pres_color;
    get_pressure_color(P, 21760000, 22528000, &pres_color.r, &pres_color.g, &pres_color.b);

    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        set_neopixel_color(pres_color, i);
    }

    format_pres_string(P, pres_buffer, max_len);

    if (text_bounds.text_is_displayed)
    {
        ili9341_clear_text();
        while (screen_draw_is_busy())
        {
            spi2_process_callbacks();
        }
    }

    ili9341_draw_string(pres_buffer, 150, 120, 0xFFFF, 0x0000);

    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    send_neopixel_data();

    bme_data_ready = false;
}

void process_and_display_bme_humidity_data(void)
{
    process_data_helper();

    char hum_buffer[15];
    uint32_t max_len = 15;

    neopixel_color_t hum_color;
    get_humidity_color(H, 0, 81920, &hum_color.r, &hum_color.g, &hum_color.b);

    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        set_neopixel_color(hum_color, i);
    }

    format_hum_string(H, hum_buffer, max_len);

    if (text_bounds.text_is_displayed)
    {
        ili9341_clear_text();
        while (screen_draw_is_busy())
        {
            spi2_process_callbacks();
        }
    }

    ili9341_draw_string(hum_buffer, 150, 120, 0xFFFF, 0x0000);

    while (screen_draw_is_busy())
    {
        spi2_process_callbacks();
    }

    send_neopixel_data();

    bme_data_ready = false;
}

void bme280_increment_display_state(void)
{
    display_state = (display_state + 1) % 4;
}

void bme280_decrement_display_state(void)
{
    display_state = (display_state + 3) % 4;
}

bme280_display_state_t get_bme280_display_state(void)
{
    return display_state;
}

void toggle_celsius_mode(void)
{
    celsius_mode = !celsius_mode;
}

bool is_in_celsius_mode(void)
{
    return celsius_mode;
}

void set_celsius_mode_changed(void)
{
    celsius_mode_changed_flag = true;
}

void reset_celsius_mode_changed(void)
{
    celsius_mode_changed_flag = false;
}

bool celsius_mode_changed(void)
{
    return celsius_mode_changed_flag;
}

// Function to kick off data collection from BME280, utilizing the I2C interface
// and custom state machine.
void start_bme_data_collection(void) 
{
    // Guard against re-entrant calls that would corrupt the I2C state machine.
    // If a read is already in progress, skip this request – the next periodic
    // poll or encoder turn will retry.
    if (I2C_read_is_busy())
    {
        return;
    }
    I2C1_master_receive(BME280_ADDR, BME280_DATA_START_REG, 8, background_data_buffer, bme_data_ready_callback);
}