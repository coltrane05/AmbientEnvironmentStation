#include <stdint.h>
#include <stdbool.h>
#include "rcc.h"
#include "gpio.h"
#include "led_state_machine.h"
#include "register_macros.h"
#include "timx.h"
#include "nvic.h"
#include "syscfg.h"
#include "exti.h"

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

void EXTI15_10_IRQHandler(void) {
    if (GET_BIT(EXTI->PR, 13)) {
        state_machine_run_iteration(&stateMachine, EV_BUTTON_PRESSED);
        SET_BIT(TIM2->EGR, 0);
        SET_BIT(EXTI->PR, 13); // For this register, you clear it by setting it to 1
    }
}

int main(void) {

    // Enable Clock for periferals
    SET_BIT(RCC->AHB1ENR, 0); // GPIO Port A
    SET_BIT(RCC->AHB1ENR, 2); // GPIO Port C
    SET_BIT(RCC->APB1ENR, 0); // TIM2
    SET_BIT(RCC->APB2ENR, 14); // SYSCFG (Bit 14)

    //TIM2 setup
    TIM2->PSC = 15999; // Prescaler value. With 16MHz clock, this gives 1ms period
    SET_BIT(TIM2->DIER, 0); // Enable Update Interrupt
    SET_BIT(TIM2->EGR, 0); // Generate an update event to load the prescaler value immediately
    TIM2->SR = 0; // Clear pending interrupt
    SET_BIT(TIM2->CR1, 0); // Enable Counter

    // Interrupt Setup
    NVIC->ISER[0] = (1U << 28); // TIM2 Interrupt is (IRQ 28)
    NVIC->ISER[1] = (1U << 8); // EXTI15_10 Interrupt is (IRQ 40)

    // See page 59 of PM0214 for instructions related to CMSIS functions
    __asm("cpsie i"); // Enable global iterrupts

    // System Config for external interrupt on line 13 Port C
    SYSCFG->EXTICR4 &= ~(0xF << 4); // Clear EXTI13 (4 bits starting at bit 4 on EXTICR4)
    SYSCFG->EXTICR4 |= (0x2 << 4); // Set EXTI13 to 2 (The setting for Port C)

    // External interrupt setup
    SET_BIT(EXTI->IMR, 13); // Unmask line 13
    SET_BIT(EXTI->FTSR, 13); // Set interrupt to trigger on falling edge.
                             // Hardware is pulled high so when you press
                             // there is a falling edge.

    // Set pin modes for Port A pin 5 and Port C pin 13
    SET_2BIT_FIELD(GPIOA->MODER, 5, 0b01); // Output Port A pin 5 User Led
    SET_2BIT_FIELD(GPIOC->MODER, 13, 0b00); // Input Port C pin 13 User Button

    state_machine_init(&stateMachine); // Initialize state machine

    while(1);
    return 0;
}