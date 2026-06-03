#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define GPIOA_BASE 0x40020000UL
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
#define GPIOA ((Custom_GPIO_t *) GPIOA_BASE)

#define GPIOC_BASE 0x40020800UL
#define GPIOC ((Custom_GPIO_t *) GPIOC_BASE)

#endif