#include "setup.h"
#include "stdint.h"
#include "register_macros.h"
#include "gpio.h"
#include "rcc.h"
#include "timx.h"
#include "nvic.h"
#include "exti.h"
#include "syscfg.h"
#include "usart.h"

void setup(void) {
    // Enable Clock for periferals
    SET_BIT(RCC->AHB1ENR, 0); // GPIO Port A
    SET_BIT(RCC->AHB1ENR, 2); // GPIO Port C
    SET_BIT(RCC->APB1ENR, 0); // TIM2
    SET_BIT(RCC->APB2ENR, 14); // SYSCFG (Bit 14)
    SET_BIT(RCC->APB1ENR, 17); // USART2 (Bit 17)

    //TIM2 setup
    TIM2->PSC = 15999; // Prescaler value. With 16MHz clock, this gives 1ms period
    SET_BIT(TIM2->DIER, 0); // Enable Update Interrupt
    SET_BIT(TIM2->EGR, 0); // Generate an update event to load the prescaler value immediately
    TIM2->SR = 0; // Clear pending interrupt
    SET_BIT(TIM2->CR1, 0); // Enable Counter

    // Interrupt Setup
    NVIC->ISER[0] = (1U << 28); // TIM2 Interrupt (IRQ 28)
    NVIC->ISER[1] = (1U << 8); // EXTI15_10 Interrupt (IRQ 40)
    NVIC->ISER[1] = (1U << 6); // USART_2 Interrupt is (IRQ 38)

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
    SET_2BIT_FIELD(GPIOA->MODER, 2, 0b10); // Alternate Function Port A pin 2 USART2_TX
    SET_2BIT_FIELD(GPIOA->MODER, 3, 0b10); // Alternate Function Port A pin 3 USART2_RX

    // Set alternate function for Port A pin 2 and pin 3 to AF7 (USART2)
    GPIOA->AFRL &= ~(0xF << 8); // Clear AFRL for pin 2
    GPIOA->AFRL |= (0x7 << 8); // Set AFRL for pin 2 to AF7 (USART2_TX)
    GPIOA->AFRL &= ~(0xF << 12); // Clear AFRL for pin 3
    GPIOA->AFRL |= (0x7 << 12); // Set AFRL for pin 3 to AF7 (USART2_RX)

    // Setup USART2
    USART2->BRR = 139U;
    SET_BIT(USART2->CR1, 3);
    SET_BIT(USART2->CR1, 2);
    SET_BIT(USART2->CR1, 13);    
}