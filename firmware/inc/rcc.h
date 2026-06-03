#ifndef RCC_H
#define RCC_H


#include <stdint.h>

#define RCC_BASE 0x40023800UL
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    uint32_t RESERVED[8];
    volatile uint32_t AHB1ENR;
} Custom_RCC_t;
#define RCC ((Custom_RCC_t *) RCC_BASE)

#endif