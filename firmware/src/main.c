#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"
#include "i2c.h"
#include "bme280.h"

stateMachine_t stateMachine;
unsigned int check_BME = 0;

int main(void) {
    setup();
    usart2_print("Booting...\r\n");
    state_machine_init(&stateMachine); // Initialize state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    bme280_init();

    while(1) {
        if(check_BME == 1) {
            int32_t temp_c = calculate_bme_temp();
            int32_t temp_f = convert_celsius_to_fahrenheit(temp_c);


            char temp_c_string[15];
            char temp_f_string[15];
            format_temp_string(temp_c, temp_c_string, sizeof(temp_c_string));
            format_temp_string(temp_f, temp_f_string, sizeof(temp_f_string));

            usart2_print("Celsius: ");
            usart2_print(temp_c_string);
            usart2_println(" C");
            
            usart2_print("Fahrenheit: ");
            usart2_print(temp_f_string);
            usart2_println(" F");
            check_BME = 0;
        }
    }
    return 0;
}