#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"
#include "i2c.h"
#include "bme280.h"
#include "systick.h"
#include "timx.h"
 
int main(void) {
    setup();
    systick_init(SYSTEM_CLOCK);
    tim2_standard_init();

    usart2_print("Booting...\r\n");
    state_machine_init();
     // Initialize led state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    bme280_init();

    while(1) 
    {
        if (get_check_BME()) 
        {
            start_bme_data_collection();
            reset_check_BME();
        }

        if (get_bme_data_ready()) 
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
            if (get_current_duty_cycle() == 100 || get_current_duty_cycle() == 0) {
                flip_led_breathing_direction();
            }

            uint16_t new_duty_cycle = get_current_duty_cycle() + get_led_breathing_direction();
            set_pwm_duty_cycle(new_duty_cycle);
            set_current_duty_cycle(new_duty_cycle);

            set_previous_systick_count(millis());
        }

        // if (millis() == 5000) 
        // {
        //     usart2_println("Welcome to 5 seconds you fool");
        // }
    }
    return 0;
}