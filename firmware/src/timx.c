#include "register_macros.h"
#include "timx.h"
#include "gpio.h"
#include "led_state_machine.h"
#include "bme280.h"


void tim2_handle_interrupt(void) {
    if (TIM2->SR & (1U << 0)) {
        TIM2->SR = 0; // Clear the update interrupt flag
        if (state_machine_is_blinking()) {
            TOGGLE_BIT(GPIOA->ODR, 5); // Toggle the LED on Port A pin 5
            // usart2_println("Hunter Sucks");
        }
        set_check_BME();
    }
}