#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"
#include "i2c.h"

stateMachine_t stateMachine;
unsigned int check_BME = 0;

int main(void) {
    setup();
    usart2_print("Booting...\r\n");
    state_machine_init(&stateMachine); // Initialize state machine

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    uint8_t i2c_chip_id_buffer[1];
    uint8_t i2c_env_data_buffer[8];
    while(1) {
        if(check_BME == 1) {
            I2C1_master_receive(0x76, 0xD0, 1, i2c_chip_id_buffer);
            // I2C1_master_receive(0x76, 0xF7, 8, i2c_env_data_buffer);
            check_BME = 0;
        }
    }
    return 0;
}