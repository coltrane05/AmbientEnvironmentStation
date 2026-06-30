#include <stdint.h>
#include "interrupt_handler.h"
#include "register_macros.h"
#include "gpio.h"
#include "timx.h"
#include "led_state_machine.h"
#include "exti.h"
#include "usart.h"
#include "i2c.h"
#include "systick.h"
#include "spi.h"
#include "neopixel_ring.h"

void Systick_Handler(void)
{
    systick_handle_interrupt();
}

// TIM2 interrupt handler. This will be called at every timer event
void TIM2_IRQHandler(void) 
{
    tim2_handle_interrupt();
}

// EXTI15_10 interrupt handler. 
// This will be called when a Falling edge on line 13 Port C is detected.
void EXTI15_10_IRQHandler(void) 
{
    exti15_10_handle_interrupt();
}

void USART2_IRQHandler(void) 
{
        usart2_handle_interrupt();
}

void I2C1_EV_IRQHandler(void) 
{
    I2C1_handle_interrupt();
}

void I2C1_ER_IRQHandler(void) 
{
    I2C1_handle_error_interrupt();
}

void DMA1_Stream4_IRQHandler(void)
{
    spi2_handle_dma_interrupt();
}

void EXTI9_5_IRQHandler(void)
{
    exti9_5_handle_interrupt();
}

void DMA1_Stream7_IRQHandler(void)
{
    neopixel_dma_handle_interrupt();
}