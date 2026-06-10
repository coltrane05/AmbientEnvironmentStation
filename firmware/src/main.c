#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"
#include "i2c.h"
#include "bme280.h"

int main(void) {
    setup();
    usart2_print("Booting...\r\n");
    state_machine_init(); // Initialize state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    bme280_init();

    char pres_buffer[15];
    char temp_buffer[15];
    char hum_buffer[15];
    char* bme_data_string_buffer[3] = {pres_buffer, temp_buffer, hum_buffer};

    while(1) {
        if(get_check_BME()) {

            get_bme_data(bme_data_string_buffer, 15);

            usart2_print("Pressure: ");
            usart2_print(bme_data_string_buffer[0]);
            usart2_println(" hPa");

            usart2_print("Temperature: ");
            usart2_print(bme_data_string_buffer[1]);
            usart2_println(" F");
            
            usart2_print("Humidity: ");
            usart2_print(bme_data_string_buffer[2]);
            usart2_println("%");
            usart2_println("");
            reset_check_BME();
        }
    }
    return 0;
}