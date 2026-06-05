#ifndef TIMX_H
#define TIMX_H

#include <stdint.h>

// Base address for TIM2. This is specific to the STM32F4 series microcontrollers.
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
    uint32_t RESERVED0;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    uint32_t RESERVED1;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR; //ONLY FOR TIM2 and TIM5
} Custom_TIMx_t;

// Define a pointer to the TIM2 structure for easy access to the TIM2 registers.
#define TIM2 ((Custom_TIMx_t *) TIM2_BASE)



#endif