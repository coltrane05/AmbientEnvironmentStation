#include <stdint.h>
#include <stdbool.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"
#include "i2c.h"
#include "bme280.h"
#include "systick.h"
#include "timx.h"
#include "spi.h"
#include "ili9341.h"
#include "fahrenheit_icon.h"
#include "celsius_icon.h"
#include "humidity_icon.h"
#include "pressure_icon.h"
#include "weather_station_icon.h"
#include "ky040.h"
#include "neopixel_ring.h"
 
int main(void) {
    setup();
    systick_init(SYSTEM_CLOCK);
    tim2_standard_init();
    spi2_init();

    usart2_print("Booting...\r\n");
    state_machine_init();
     // Initialize led state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    bme280_init();

    ili9341_reset();
    ili9341_init();

    ky040_init();

    neopixel_init();

    uint32_t last_rgb_animation_increment = 0;
    bool rgb_animation_active = false;

    uint32_t last_bme280_read = 0;
    start_bme_data_collection();

    const ili9341_colors colors = ILI9341_COLORS;
    ili9341_fill_screen(colors.BLACK);

    while(1) 
    {
        spi2_process_callbacks();

        if (millis() - last_bme280_read > 2000)
        {
            start_bme_data_collection();
            last_bme280_read = millis();
        }

        if (celsius_mode_changed())
        {
            start_bme_data_collection();
            last_bme280_read = millis();
            reset_celsius_mode_changed();
        }

        if (bme_data_is_ready() && !screen_draw_is_busy()) 
        {
            if (get_bme280_display_state() == BME280_TEMP)
            {
                if (is_in_celsius_mode()){
                    ili9341_draw_icon(celsius_icon, CELSIUS_WIDTH * CELSIUS_HEIGHT, 15, 60, CELSIUS_WIDTH, CELSIUS_HEIGHT);
                }
                else
                {
                    ili9341_draw_icon(fahrenheit_icon, FAHRENHEIT_WIDTH * FAHRENHEIT_HEIGHT, 15, 60, FAHRENHEIT_WIDTH, FAHRENHEIT_HEIGHT);
                }

                while (screen_draw_is_busy())
                {
                    spi2_process_callbacks();
                }

                process_and_display_bme_temperature_data();

                rgb_animation_active = false;
            }
            else if (get_bme280_display_state() == BME280_PRES)
            {
                ili9341_draw_icon(pressure_icon, PRESSURE_WIDTH * PRESSURE_HEIGHT, 15, 60, PRESSURE_WIDTH, PRESSURE_HEIGHT);

                while (screen_draw_is_busy())
                {
                    spi2_process_callbacks();
                }

                process_and_display_bme_pressure_data();

                rgb_animation_active = false;
            }
            else if (get_bme280_display_state() == BME280_HUM)
            {
                ili9341_draw_icon(humidity_icon, HUMIDITY_WIDTH * HUMIDITY_HEIGHT, 15, 60, HUMIDITY_WIDTH, HUMIDITY_HEIGHT);

                while (screen_draw_is_busy())
                {
                    spi2_process_callbacks();
                }

                process_and_display_bme_humidity_data();

                rgb_animation_active = false;
            }
            else
            {
                ili9341_draw_icon(weather_station_icon, WEATHER_STATION_WIDTH * WEATHER_STATION_HEIGHT, 15, 60, WEATHER_STATION_WIDTH, WEATHER_STATION_HEIGHT);

                while (screen_draw_is_busy())
                {
                    spi2_process_callbacks();
                }

                process_and_display_all_bme_data();

                rgb_animation_active = true;
            }
        }

        if (rgb_animation_active && (millis() - last_rgb_animation_increment > 1))
        {
            last_rgb_animation_increment = millis();
            set_rgb_animation_colors();
            send_neopixel_data();
            increment_rgb_animation_colors();
        }

        if (read_char_is_ready()) 
        {
            print_read_char();
        }

        if (read_buffer_is_ready()) 
        {
            process_read_buffer();
        }

        if (state_machine_is_dimming() && (millis() - get_previous_systick_count() >= 50)) 
        {
            if (get_current_duty_cycle() == 100 || get_current_duty_cycle() == 0) 
            {
                flip_led_breathing_direction();
            }

            uint16_t new_duty_cycle = get_current_duty_cycle() + get_led_breathing_direction();
            set_tim2_pwm_duty_cycle(new_duty_cycle);
            set_current_duty_cycle(new_duty_cycle);

            set_previous_systick_count(millis());
        }

        if (change_state_forward_flag_is_set())
        {
            bme280_increment_display_state();
            start_bme_data_collection();
            last_bme280_read = millis();
            reset_change_state_forward_flag();
        }

        if (change_state_backward_flag_is_set())
        {
            bme280_decrement_display_state();
            start_bme_data_collection();
            last_bme280_read = millis();
            reset_change_state_backward_flag();
        }
    }
    return 0;
}