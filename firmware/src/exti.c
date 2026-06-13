#include "exti.h"
#include "timx.h"
#include "led_state_machine.h"
#include "register_macros.h"

void exti15_10_handle_interrupt(void) {
    if (GET_BIT(EXTI->PR, 13)) {
        state_machine_run_iteration(EV_BUTTON_PRESSED);
        SET_BIT(TIM2->EGR, 0);
        EXTI->PR = (1U << 13); // Write 1 to clear as specified by the reference manual
    }
}