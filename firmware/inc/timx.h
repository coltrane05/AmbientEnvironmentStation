#ifndef TIMX_H
#define TIMX_H

#include <stdint.h>

// Base addresses for timers. This is specific to the STM32F4 series microcontrollers.
#define TIM1_BASE 0x40010000
#define TIM2_BASE 0x40000000

// A structure to represent the TIMx registers.
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR; //ONLY FOR TIM2 and TIM5
} custom_timx_t;

// Define a pointer to the TIMx structure for easy access to the TIMx registers.
#define TIM1 ((custom_timx_t *) TIM1_BASE)
#define TIM2 ((custom_timx_t *) TIM2_BASE)

void tim2_handle_interrupt (void);
void tim2_pwm_init (void);
void tim2_standard_init (void);
void set_pwm_duty_cycle (uint16_t duty_cycle);


#endif