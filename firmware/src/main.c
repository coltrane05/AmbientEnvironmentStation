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
    while(1) {
        if(check_BME == 1) {
            I2C1_read(0x76, 0xD0);
            check_BME = 0;
        }
    }
    return 0;
}