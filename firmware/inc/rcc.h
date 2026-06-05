#ifndef RCC_H
#define RCC_H


#include <stdint.h>

// Base address for the Reset and Clock Control (RCC) registers. 
// This is specific to the STM32F4 series microcontrollers.
#define RCC_BASE 0x40023800UL

// A structure to represent the RCC registers.
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    uint32_t RESERVED0;
    uint32_t RESERVED1;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED2;
    uint32_t RESERVED3;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    uint32_t RESERVED4;
    uint32_t RESERVED5;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    uint32_t RESERVED6;
    uint32_t RESERVED7;
} Custom_RCC_t;

// Define a pointer to the RCC structure for easy access to the RCC registers.
#define RCC ((Custom_RCC_t *) RCC_BASE)

#endif