#ifndef USART_H
#define USART_H

#include <stdint.h>

#define USART2_BASE 0x40004400UL

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} Custom_USART_t;

#define USART2 ((Custom_USART_t *) USART2_BASE)

#endif