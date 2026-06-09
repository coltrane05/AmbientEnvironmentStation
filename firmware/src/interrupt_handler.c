#include <stdint.h>
#include "interrupt_handler.h"
#include "register_macros.h"
#include "gpio.h"
#include "timx.h"
#include "led_state_machine.h"
#include "exti.h"
#include "usart.h"
#include "i2c.h"

// This variable is declared in main
extern stateMachine_t stateMachine;
extern volatile unsigned int check_BME;

// TIM2 interrupt handler. This will be called at every timer event
void TIM2_IRQHandler(void) {
    if (TIM2->SR & (1U << 0)) {
        TIM2->SR = 0; // Clear the update interrupt flag
        if (stateMachine.currState != ST_LED_OFF && stateMachine.currState != ST_LED_SOLID) {
            TOGGLE_BIT(GPIOA->ODR, 5); // Toggle the LED on Port A pin 5
            // usart2_println("Hunter Sucks");
        }
        check_BME = 1;
    }
}

// EXTI15_10 interrupt handler. 
// This will be called when a Falling edge on line 13 Port C is detected.
void EXTI15_10_IRQHandler(void) {
    if (GET_BIT(EXTI->PR, 13)) {
        state_machine_run_iteration(&stateMachine, EV_BUTTON_PRESSED);
        SET_BIT(TIM2->EGR, 0);
        EXTI->PR = (1U << 13); // Write 1 to clear. Do NOT use read-modify-write (|=) here!
    }
}