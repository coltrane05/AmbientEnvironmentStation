#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

// Base address for the Flash memory interface registers.
// This is specific to the STM32F4 series microcontrollers.
#define FLASH_BASE 0x40023C00UL

// A structure to represent the Flash registers.
typedef struct {
    volatile uint32_t ACR;      // Flash access control register
    volatile uint32_t KEYR;     // Flash key register, Address offset: 0x04
    volatile uint32_t OPTKEYR;  // Flash option key register, Address offset: 0x08
    volatile uint32_t SR;       // Flash status register, Address offset: 0x0C
    volatile uint32_t CR;       // Flash control register, Address offset: 0x10
    volatile uint32_t OPTCR;    // Flash option control register, Address offset: 0x14
} custom_flash_t;

// Define a pointer to the FLASH structure for easy access to the Flash registers.
#define FLASH ((custom_flash_t *) FLASH_BASE)

#endif