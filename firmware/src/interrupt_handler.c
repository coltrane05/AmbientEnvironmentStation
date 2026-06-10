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

// TIM2 interrupt handler. This will be called at every timer event
void TIM2_IRQHandler(void) {
    tim2_handle_interrupt();
}

// EXTI15_10 interrupt handler. 
// This will be called when a Falling edge on line 13 Port C is detected.
void EXTI15_10_IRQHandler(void) {
    exti15_10_handle_interrupt();
}

void USART2_IRQHandler(void) {
    if((USART2->SR & (1 << 7))) {
        usart2_handle_interrupt();
    }
}