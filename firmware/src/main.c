#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"
#include "usart.h"

stateMachine_t stateMachine;

int main(void) {
    setup();
    usart2_print("Booting...\r\n");
    state_machine_init(&stateMachine); // Initialize state machine

    while(1);
    return 0;
}