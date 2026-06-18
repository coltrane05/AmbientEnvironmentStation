#include "rcc.h"
#include "flash.h"

void rcc_init(void) 
{
    while(!(RCC->CR & (1 << 1))); // Wait for HSI to be ready

    // Configure Flash latency (Wait States) to 2WS for 84MHz
    FLASH->ACR &= ~0xF;     // Clear latency bits
    FLASH->ACR |= 0x2;      // Set latency to 2 wait states

    // Clear PLLM, PLLN, and PLLP bits before setting them
    RCC->PLLCFGR &= ~((0x3F << 0) | (0x1FF << 6) | (0x3 << 16));

    RCC->PLLCFGR |= (8 << 0); // PLLM division factor to 8
    RCC->PLLCFGR |= (168 << 6); // PLLN multiplication factor to 168
    RCC->PLLCFGR |= (0b01 << 16); // PLLP division factor to 4

    // Set APB1 prescaler to /2 since max APB1 frequency is 42MHz
    // Bits 12:10 in CFGR. 0b100 is divide by 2.
    RCC->CFGR &= ~(0x7 << 10);
    RCC->CFGR |= (0b100 << 10);

    RCC->CR |= (1 << 24); // Enable the PLL
    while(!(RCC->CR & (1 << 25))); // Wait for PLL

    RCC->CFGR &= ~(0b11 << 0); // Clear SW bits
    RCC->CFGR |= (0b10 << 0); // Set PLL as system clock source
    while ((RCC->CFGR & (0b11 << 2)) != (0b10 << 2)); // Wait until PLL is used
}