#ifndef SYSGFG_H
#define SYSGFG_H

#include <stdint.h>

// Base address for the SYSGFG register
#define SYSGFG_BASE 0x40013800UL

// A structure to represent SYSGFG registers 
typedef struct 
{
    volatile uint32_t MEMRMP;
    volatile uint32_t PMC;
    volatile uint32_t EXTICR1;
    volatile uint32_t EXTICR2;
    volatile uint32_t EXTICR3;
    volatile uint32_t EXTICR4;
    uint32_t RESERVED0;
    uint32_t RESERVED1;
    volatile uint32_t CMPCR;
} Custom_SYSCFG_t;

// Define pointers to the SYSCFG structure.
#define SYSCFG ((Custom_SYSCFG_t *) SYSGFG_BASE)

#endif