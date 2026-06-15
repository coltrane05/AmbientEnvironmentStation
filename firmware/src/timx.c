#include "register_macros.h"
#include "timx.h"
#include "gpio.h"
#include "led_state_machine.h"
#include "bme280.h"


void tim2_handle_interrupt(void) {
    if (TIM2->SR & (1U << 0)) {
        TIM2->SR = 0; // Clear the update interrupt flag
        if (state_machine_is_blinking()) {
            TOGGLE_BIT(GPIOA->ODR, 5); // Toggle the LED on Port A pin 5
        }
    }
}

void tim2_pwm_init(void)
{
    TIM2->CR1 &= ~(1 << 0);
    TIM2->CCER &= ~(1 << 0);

    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |= (0b10 << (5 * 2));

    TIM2->PSC = 0; // Timer frequecy = system clock frequency
    TIM2->ARR = 6399; // 2.5kHz period
    TIM2->CCR1 = 3200;

    TIM2->CCMR1 &= ~(0b111 << 4);
    TIM2->CCMR1 |= (0b110 << 4);
    TIM2->CCMR1 |= (1 << 3);

    TIM2->EGR |= (1 << 0);

    TIM2->CCER |= (1 << 0);
    TIM2->CR1 |= (1 << 0);

}

void tim2_standard_init(void)
{  
    TIM2->CR1 &= ~(1 << 0);
    TIM2->CCER &= ~(1 << 0);

    GPIOA->MODER &= ~(0b11 << (5 * 2));
    GPIOA->MODER |= (0b01 << (5 * 2));

    TIM2->PSC = (16000000 / 1000) - 1;
    TIM2->ARR = 1500;
    
    TIM2->DIER |= (1 << 0);
    TIM2->EGR |= (1 << 0);
    TIM2->SR = 0;
    
    TIM2->CR1 |= (1 << 0);
}

void set_pwm_duty_cycle(uint16_t duty_cycle)
{
    uint16_t new_duty_cycle = (6399 * duty_cycle) / 100;
    TIM2->CCR1 = new_duty_cycle;
}