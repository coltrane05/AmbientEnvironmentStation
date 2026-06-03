#include <stdint.h>
#include <stdbool.h>
#include "rcc.h"
#include "gpio.h"
#include "led_state_machine.h"

unsigned int countdown_clicks;


void dummy_delay(unsigned int clicks) {
    while(clicks-- > 0) {
        if (!(GPIOC->IDR >> 13) & 1) {
            break;
        }
        __asm("nop");
    }
}

int main(void) {

    // Enable Clock for GPIO ports A and C
    RCC->AHB1ENR |= (1 << 0); // Port A
    RCC->AHB1ENR |= (1 << 2); // Port C

    // Set pin modes for Port A pin 5 and Port C pin 13
    GPIOA->MODER |= (0b01 << (2 * 5)); // Output Port A pin 5 User Led
    GPIOC->MODER |= (0b00 << (2 * 13)); // Input Port C pin 13 User Button

    stateMachine_t stateMachine;

    state_machine_init(&stateMachine);

    while(1) {

        
        if (!(GPIOC->IDR >> 13) & 0x1) {
            state_machine_run_iteration(&stateMachine, EV_BUTTON_PRESSED, &countdown_clicks);
            for (int i = 0; i < 500000; i++) {
                __asm("nop");
            }
        }
        if (stateMachine.currState != ST_LED_OFF) {
            if (stateMachine.currState != ST_LED_SOLID) {
                dummy_delay(countdown_clicks);
                GPIOA->ODR ^= (1 << 5);
            }
        }
    }
    return 0;
}