#include "spi.h"
#include "rcc.h"
#include "gpio.h"

void spi2_init(void)
{
    RCC->APB1ENR |= (1 << 14); // Enable clock for SPI2

    // Set output pin mode for DC and Reset pins on ILI9341
    GPIOB->MODER &= ~(0b11 << (10 * 2)); // Port B pin 10 for ILI9341 DC pin
    GPIOB->MODER |= (0b01 << (10 * 2));
    GPIOB->MODER &= ~(0b11 << (8 * 2)); // Port B pin 8 for ILI9341 reset pin
    GPIOB->MODER |= (0b01 << (8 * 2)); 
    
    GPIOB->MODER &= ~(0b11 << (12 * 2)); // Clear Port B pin 12 mode
    GPIOB->MODER |= (0b01 << (12 * 2)); // Set Port B pin 12 to Output mode (SPI_CS)
    GPIOB->MODER &= ~(0b11 << (13 * 2)); // Clear Port B pin 13 mode
    GPIOB->MODER |= (0b10 << (13 * 2)); // Set Port B pin 13 to AF mode (SPI_SCK)
    GPIOB->MODER &= ~(0b11 << (14 * 2)); // Clear Port B pin 14 mode
    GPIOB->MODER |= (0b10 << (14 * 2)); // Set Port B pin 14 to AF mode (SPI_MISO)
    GPIOB->MODER &= ~(0b11 << (15 * 2)); // Clear Port B pin 15 mode
    GPIOB->MODER |= (0b10 << (15 * 2)); // Set Port B pin 15 to AF mode (SPI_MOSI)

    GPIOB->AFRH &= ~(0xF << ((13 - 8) * 4)); // Clear Port B pin 13 AF
    GPIOB->AFRH |= (0x5 << ((13 - 8) * 4)); // Set Port B pin 13 to AF05
    GPIOB->AFRH &= ~(0xF << ((14 - 8) * 4)); // Clear Port B pin 14 AF
    GPIOB->AFRH |= (0x5 << ((14 - 8) * 4)); // Set Port B pin 14 to AF05
    GPIOB->AFRH &= ~(0xF << ((15 - 8) * 4)); // Clear Port B pin 15 AF
    GPIOB->AFRH |= (0x5 << ((15 - 8) * 4)); // Set Port B pin 15 to AF05

    SPI2->CR1 |= (1 << 9); // Enable software slave management
    SPI2->CR1 |= (1 << 8); // Set internal slave select (SSI). Required for master mode when SSM=1
    SPI2->CR1 |= (0b001 << 3); // 16MHz/4 = 4MHz
    SPI2->CR1 |= (1 << 2); // Set to master

    GPIOB->ODR |= (1 << 12); // Set chip select high
    GPIOB->ODR |= (1 << 8); // Set reset pin high
    SPI2->CR1 |= (1 << 6); // Enable spi.
}

void spi2_write(uint8_t * data_buffer, uint32_t buffer_size)
{
    GPIOB->ODR &= ~(1 << 12); // Pull CS pin low

    for (uint32_t i = 0; i < buffer_size; i++) 
    {
        while(!(SPI2->SR & (1 << 1)));
        SPI2->DR = data_buffer[i];
    }

    while(!(SPI2->SR & (1 << 1))); // Wait for TXE to ensure the last byte is moved to the shift register
    while((SPI2->SR & (1 << 7)));  // Wait for BSY (Busy) flag to clear to ensure physical transmission is done

    GPIOB->ODR |= (1 << 12); // Pull CS pin back high indicating end of trasmission
}