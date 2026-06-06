#include <stdint.h>
#include "led_state_machine.h"
#include "setup.h"
#include "interrupt_handler.h"

stateMachine_t stateMachine;

int main(void) {

    setup();

    state_machine_init(&stateMachine); // Initialize state machine

    // TODO: Implement USART2 interrupt handler to send current state over serial when button is pressed
    // Create a RingBuffer to hold the string representation of the current state and send it over
    // USART2 when the button is pressed. 
    

    while(1);
    return 0;
}