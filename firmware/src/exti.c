#include "exti.h"
#include "timx.h"
#include "led_state_machine.h"
#include "register_macros.h"
#include "ky040.h"
#include "bme280.h"

void exti15_10_handle_interrupt(void) 
{
    if (GET_BIT(EXTI->PR, 13)) 
    {
        state_machine_run_iteration(EV_BUTTON_PRESSED);
        SET_BIT(TIM2->EGR, 0);
        EXTI->PR = (1U << 13); // Write 1 to clear as specified by the reference manual
    }
}

void exti9_5_handle_interrupt(void)
{
    if (GET_BIT(EXTI->PR, 8) || GET_BIT(EXTI->PR, 9))
    {
        ky040_determine_change_direction();
        EXTI->PR = (3U << 8); // 8 and 9 are right next to each other.
                              // writing 3 clears both bits.
    }

    if (GET_BIT(EXTI->PR, 6))
    {
        toggle_celsius_mode();
        set_celsius_mode_changed();
        EXTI->PR = (1U << 6); // Write 1 to clear pending bit
    }
}