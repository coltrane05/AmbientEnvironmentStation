#ifndef TIMX_H
#define TIMX_H

#include <stdint.h>

#define TIM2_BASE 0x40000000

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

#define TIM2 ((Custom_TIMx_t *) TIM2_BASE)



#endif