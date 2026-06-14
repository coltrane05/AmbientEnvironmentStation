#include <stdint.h>

// This file contains the startup code for the STM32F401RE microcontroller.
// It defines the interrupt vector table, the default handlers for interrupts
// and the Reset_Handler which initializes the .data and .bss sections before calling main().

// The symbols _estack, _etext, _edata, _sdata, _sbss, and _ebss are defined in the linker script.
extern uint32_t _estack, _etext, _edata, _sdata, _sbss, _ebss;


int main (void);

// Forward declaration of the default handlers for interrupts.
// The __attribute__((weak, alias("Default_Handler"))) syntax allows us to define a 
// default handler for each interrupt, which can be overridden by the user if needed. 
// If the user does not provide a specific handler for an interrupt, the Default_Handler will be used, 
// which simply enters an infinite loop, allowing the user to identify that an unexpected interrupt has occurred
// using OpenOCD or a similar debugging tool.
void Reset_Handler (void);
void Default_Handler (void);
void NMI_Handler (void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler (void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler (void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler (void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler (void) __attribute__((weak, alias("Default_Handler")));
void SVCall_Handler (void) __attribute__((weak, alias("Default_Handler")));
void Debug_Monitor_Handler (void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler (void) __attribute__((weak, alias("Default_Handler")));
void Systick_Handler (void) __attribute__((weak, alias("Default_Handler")));
void WWDG_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void SPI4_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));

// The interrupt vector table. This is an array of function pointers that the
// processor uses to determine which function to call when an interrupt occurs.
// The __attribute__((section(".isr_vector"))) syntax tells the linker to place this vector table
// at the beginning of the flash memory, which is where the processor expects to find it.
__attribute__((section(".isr_vector")))
void (* const vector_table[])(void) = {
    (void (*)(void))(&_estack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVCall_Handler,
    Debug_Monitor_Handler,
    0,
    PendSV_Handler,
    Systick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMP_STAMP_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Stream0_IRQHandler,
    DMA1_Stream1_IRQHandler,
    DMA1_Stream2_IRQHandler,
    DMA1_Stream3_IRQHandler,
    DMA1_Stream4_IRQHandler,
    DMA1_Stream5_IRQHandler,
    DMA1_Stream6_IRQHandler,
    ADC_IRQHandler,
    0,
    0,
    0,
    0,
    EXTI9_5_IRQHandler,
    TIM1_BRK_TIM9_IRQHandler,
    TIM1_UP_TIM10_IRQHandler,
    TIM1_TRG_COM_TIM11_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    0,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    OTG_FS_WKUP_IRQHandler,
    0,
    0,
    0,
    0,
    DMA1_Stream7_IRQHandler,
    0,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    0,
    0,
    0,
    0,
    DMA2_Stream0_IRQHandler,
    DMA2_Stream1_IRQHandler,
    DMA2_Stream2_IRQHandler,
    DMA2_Stream3_IRQHandler,
    DMA2_Stream4_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    OTG_FS_IRQHandler,
    DMA2_Stream5_IRQHandler,
    DMA2_Stream6_IRQHandler,
    DMA2_Stream7_IRQHandler,
    USART6_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    FPU_IRQHandler,
    0,
    0,
    SPI4_IRQHandler
};

// The Default_Handler is a catch-all for any interrupts that are not specifically handled by the user.
void Default_Handler (void)
{
    while (1);
}

// The Reset_Handler is called when the microcontroller is reset. 
// It initializes the .data and .bss sections before calling main().
void Reset_Handler (void)
{
    uint32_t * flash_data_src = &_etext;
    uint32_t * ram_data_dest = &_sdata;

    while (ram_data_dest < &_edata)
    {
        *ram_data_dest++ = *flash_data_src++;
    }

    uint32_t * bss_dest = &_sbss;

    while (bss_dest < &_ebss)
    {
        *bss_dest++ = 0;
    }

    main();

    while (1);
}