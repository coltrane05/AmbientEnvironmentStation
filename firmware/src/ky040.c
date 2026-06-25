#include <stdint.h>
#include "ky040.h"
#include "gpio.h"
#include "ili9341.h"

#define KY040_COUNTS_PER_DETENT  4

static uint8_t prevState;
static int8_t  cycleCount;

const int8_t encoder_table[16] = {
    // prev == 00
    0, -1, 1, 0,
    // prev == 01
    1, 0, 0, -1,
    // prev == 10
    -1, 0, 0, 1,
    // prev == 11
    0, 1, -1, 0
}; 

void ky040_init(void)
{
    uint8_t a = (GPIOC->IDR >> 8) & 1U;
    uint8_t b = (GPIOC->IDR >> 9) & 1U;
    prevState  = (a << 1) | b;
    cycleCount = 0;
}

void ky040_determine_change_direction(void)
{
    uint8_t a = (GPIOC->IDR >> 8) & 1U;
    uint8_t b = (GPIOC->IDR >> 9) & 1U;
    uint8_t currentState = (a << 1) | b;

    if (currentState == prevState)
    {
        return;
    }

    int8_t index     = (int8_t)((prevState << 2) | currentState);
    int8_t direction = encoder_table[index];

    if (direction == 0)
    {
        return;
    }

    prevState = currentState;
    cycleCount += direction;

    if (cycleCount >= KY040_COUNTS_PER_DETENT)
    {
        set_change_state_forward_flag();
        cycleCount = 0;
    }
    else if (cycleCount <= -KY040_COUNTS_PER_DETENT)
    {
        set_change_state_backward_flag();
        cycleCount = 0;
    }
}