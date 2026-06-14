#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#define STK_BASE 0xE000E010UL

typedef struct 
{
    volatile uint32_t CTRL;  // SysTick control and stutus register
    volatile uint32_t LOAD;  // SysTick reload value register
    volatile uint32_t VAL;   // SysTick current value register
    volatile uint32_t CALIB; // SysTick calibration register
} custom_stk_t;

#define STK ((custom_stk_t *) STK_BASE)

void systick_init (const uint32_t system_clock);
uint32_t millis(void);
void systick_handle_interrupt(void);

#endif