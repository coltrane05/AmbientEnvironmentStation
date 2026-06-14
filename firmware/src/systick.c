#include "systick.h"

static uint32_t systick_counter = 0;

void systick_init(const uint32_t system_clock)
{
    STK->LOAD = (system_clock / 1000) - 1; // Calculation for a 1ms count.
    STK->VAL = 1; //Writing any value to this register resets it.
    STK->CTRL |= (1 << 2); // Use processor clock.
    STK->CTRL |= (1 << 1); // SysTick exception request enable.
    STK->CTRL |= (1 << 0); // Counter enable.
}

// Returns the number of ms have passed since reset.
uint32_t millis(void)
{
    return systick_counter;
}

// Gets called in Systic_Handler()
void systick_handle_interrupt(void)
{
    systick_counter++;
}