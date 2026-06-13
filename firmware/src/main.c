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
    state_machine_init(); // Initialize led state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    bme280_init();

    while(1) {
        if(get_check_BME()) {
            start_bme_data_collection();
            reset_check_BME();
        }

        if(get_bme_data_ready()) {
            process_and_print_bme_data();
        }

        if(read_char_is_ready()) {
            print_read_char();
        }
        if(read_buffer_is_ready()) {
            print_read_buffer();
        }
    }
    return 0;
}