#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// Base addresses for GPIO ports. These are specific to the STM32F4 series microcontrollers.
#define GPIOA_BASE 0x40020000UL
#define GPIOB_BASE 0x40020400UL
#define GPIOC_BASE 0x40020800UL

// A structure to represent the GPIO registers.
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
} Custom_GPIO_t;

// Define pointers to the GPIO structures for Port A and Port C.
#define GPIOA ((Custom_GPIO_t *) GPIOA_BASE)
#define GPIOB ((Custom_GPIO_t *) GPIOB_BASE)
#define GPIOC ((Custom_GPIO_t *) GPIOC_BASE)

#endif