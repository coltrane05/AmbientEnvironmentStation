#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>

// Base address for the EXTI register
#define EXTI_BASE 0x40013C00UL

// A structure to represent EXTI registers
typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} Custom_EXTI_t;

// Define pointers to the EXTI structure.
#define EXTI ((Custom_EXTI_t *) EXTI_BASE)

void exti15_10_handle_interrupt(void);

#endif