#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

// Base address for the Nested Vectored Interrupt Controller (NVIC).
// This is specific to ARM Cortex-M microcontrollers.
#define NVIC_BASE 0xE000E100UL

// A structure to represent the NVIC registers. 
// This structure is based on the ARM Cortex-M4 NVIC.
typedef struct {
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8]; // Interrupt Clear-Enable Registers
    uint32_t RSERVED1[24];
    volatile uint32_t ISPR[8]; // Interrupt Set-Pending Registers
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8]; // Interrupt Clear-Pending Registers
    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8]; // Interrupt Active Bit Registers
    uint32_t RESERVED4[56];
    volatile uint8_t  IP[240]; 
} Custom_NVIC_t;

// Define a pointer to the NVIC structure for easy access to the NVIC registers.
#define NVIC ((Custom_NVIC_t *) NVIC_BASE)


#endif