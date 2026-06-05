#include <stdint.h>
#include <stdbool.h>
#include "rcc.h"
#include "gpio.h"
#include "led_state_machine.h"
#include "register_macros.h"
#include "timx.h"
#include "nvic.h"

stateMachine_t stateMachine;

// TIM2 interrupt handler. This will be called at every timer event
void TIM2_IRQHandler(void) {
    if (TIM2->SR & (1U << 0)) {
        TIM2->SR = 0; // Clear the update interrupt flag
        if (stateMachine.currState != ST_LED_OFF && stateMachine.currState != ST_LED_SOLID) {
            TOGGLE_BIT(GPIOA->ODR, 5); // Toggle the LED on Port A pin 5
        }
    }
}

void dummy_delay(unsigned int clicks) {
    while(clicks-- > 0) {
        if (!((GPIOC->IDR >> 13) & 1)) {
            break;
        }
        __asm("nop");
    }
}

int main(void) {

    // Enable Clock for periferals
    SET_BIT(RCC->AHB1ENR, 0); // GPIO Port A
    SET_BIT(RCC->AHB1ENR, 2); // GPIO Port C
    SET_BIT(RCC->APB1ENR, 0); // TIM2

    //TIM2 setup
    TIM2->PSC = 15999; // Prescaler value. With 16MHz clock, this gives 1ms period
    SET_BIT(TIM2->DIER, 0); // Enable Update Interrupt
    SET_BIT(TIM2->EGR, 0); // Generate an update event to load the prescaler value immediately
    TIM2->SR = 0; // Clear pending interrupt
    SET_BIT(TIM2->CR1, 0); // Enable Counter
    NVIC->ISER[0] = (1U << 28); // TIM2 Interrupt is (IRQ 28)

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    // Set pin modes for Port A pin 5 and Port C pin 13
    SET_2BIT_FIELD(GPIOA->MODER, 5, 0b01); // Output Port A pin 5 User Led
    SET_2BIT_FIELD(GPIOC->MODER, 13, 0b00); // Input Port C pin 13 User Button

    state_machine_init(&stateMachine); // Initialize state machine

    while(1) {
        if (!GET_BIT(GPIOC->IDR, 13)) {
            state_machine_run_iteration(&stateMachine, EV_BUTTON_PRESSED);
            for (int i = 0; i < 500000; i++) {
                __asm("nop");
            }
            SET_BIT(TIM2->EGR, 0); // Generate an update event to reset timer.
        }
    }
    return 0;
}