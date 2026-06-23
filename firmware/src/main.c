#include <stdint.h>
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
#include "fahrenheit.h"
#include "pressure.h"
 
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

    // ili9341_draw_pixel(120, 160, 0xF800);

    const ili9341_colors colors = ILI9341_COLORS;
    ili9341_fill_screen(colors.BLACK);

    uint8_t inc = 0;

    while(1) 
    {
        spi2_process_callbacks();
        if (get_check_BME()) 
        {
            start_bme_data_collection();
            reset_check_BME();
        }

        if (color_change_is_ready() && !screen_draw_is_busy())
        {
            if (inc == 0) 
            {
                ili9341_fill_screen(colors.BLACK);
                inc = 1;
            }
            else if (inc == 1) 
            {
                ili9341_draw_icon(fahrenheit, FAHRENHEIT_WIDTH * FAHRENHEIT_HEIGHT, 20, 60, FAHRENHEIT_WIDTH, FAHRENHEIT_HEIGHT);
                inc = 2;
            }
            else if (inc == 2) {
                ili9341_draw_icon(pressure, PRESSURE_WIDTH * PRESSURE_HEIGHT, 20, 60, PRESSURE_WIDTH, PRESSURE_HEIGHT);
                inc = 3;
            }
            else 
            {
                ili9341_draw_string("868.18 hPa", 20, 30, 0xFFFF, 0x0000);
                inc = 1;
            }


            reset_color_change_ready();
        }

        if (icon_flag_is_set() && !screen_draw_is_busy())
        {
            ili9341_draw_icon(fahrenheit, FAHRENHEIT_WIDTH * FAHRENHEIT_HEIGHT, 20, 60, FAHRENHEIT_WIDTH, FAHRENHEIT_HEIGHT);
            reset_icon_flag();
        }

        if (text_flag_is_set() && !screen_draw_is_busy())
        {
            ili9341_draw_string("868.18 hPa", 20, 30, 0xFFFF, 0x0000);
            reset_text_flag();
        }

        if (clear_text_is_set() && !screen_draw_is_busy())
        {
            ili9341_clear_text();
            reset_clear_text_flag();
        }

        if (bme_data_is_ready()) 
        {
            process_and_print_bme_data();
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
            set_pwm_duty_cycle(new_duty_cycle);
            set_current_duty_cycle(new_duty_cycle);

            set_previous_systick_count(millis());
        }
    }
    return 0;
}