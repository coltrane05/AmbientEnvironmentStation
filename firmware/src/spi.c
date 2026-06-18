#include "spi.h"
#include "rcc.h"
#include "gpio.h"
#include "dma.h"
#include "usart.h"

void spi2_init(void)
{
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
    SPI2->CR1 |= (0b000 << 3); // 42MHz/2 = 21b MHz
    SPI2->CR1 |= (1 << 2); // Set to master

    GPIOB->ODR |= (1 << 12); // Set chip select high
    GPIOB->ODR |= (1 << 8); // Set reset pin high
    SPI2->CR1 |= (1 << 6); // Enable spi.

    // Configure DMA
    DMA1->S4CR &= ~(0b111 << 25); // Ensure Channel Selection is 0
    DMA1->S4CR |= (0b01 << 6); // Set Direction to Memory-to-Peripheral
    DMA1->S4CR |= (1U << 10); // Memory increment mode
    DMA1->S4CR &= ~(1U << 9); // Ensure peripheral increment mode is disabled
    DMA1->S4CR &= ~(0b11 << 11); // Ensure peripheral data size of 8 bits
    DMA1->S4CR &= ~(0b11 << 13); // Ensure memory data size of 8 bits
    SPI2->CR2 |= (1U << 1); // Enable DMA request
}

void spi2_write(const uint8_t * data_buffer, uint32_t buffer_size)
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

void spi2_dma_write16(const uint16_t * data_buffer, uint32_t buffer_size)
{
    // Ensure DMA stream is disable before configuring
    DMA1->S4CR &= ~(1U << 0);
    while (DMA1->S4CR & (1U << 0)); // Wait until EN bti is fully cleared

    while((SPI2->SR & (1 << 7))); // Ensure SPI2 is not busy
    SPI2->CR1 &= ~(1 << 6); //Disable SPI to make config changes
    SPI2->CR1 |= (1 << 11); // Set data frame format to 16 bits
    SPI2->CR1 |= (1 << 6); // Re-enable SPI

    DMA1->S4CR |= (0b01 << 11); // Peripheral data size: 16 bits
    DMA1->S4CR |= (0b01 << 13); // Memory data size: 16 bits

    DMA1->HIFCR = (0b111101 << 0); // Clear all IFCR Flags for Stream 4 (write-1-to-clear)

    DMA1->S4PAR = (uint32_t)&SPI2->DR; // Set DMA peripheral address
    DMA1->S4M0AR = (uint32_t)data_buffer;

    if (buffer_size > 65535) {
        usart2_println("From DMA write16: BUFFER SIZE TOO LARGE");
        return;
    }

    GPIOB->ODR &= ~(1 << 12); // pull CS pin low to prepare for transmission

    DMA1->S4NDTR = (uint16_t)buffer_size;
    DMA1->S4CR |= (1U << 0);               // Enable DMA stream

    // Wait for DMA transfer to complete
    while (!(DMA1->HISR & (1U << 5)));     // Wait for TCIF (Transfer Complete)
    DMA1->HIFCR = (0b111101 << 0);         // Clear Stream 4 flags

    // Wait for SPI shift register to finish transmitting the last word
    while((SPI2->SR & (1 << 7)));          // Wait for BSY to clear

    GPIOB->ODR |= (1 << 12); // Set CS high to end transaction

    SPI2->CR1 &= ~(1 << 6); // Disable SPI to make config changes
    SPI2->CR1 &= ~(1 << 11); // Clear DFF bit to return to 8-bit format
    SPI2->CR1 |= (1 << 6); // Enable SPI again

    DMA1->S4CR &= ~(0b11 << 11); // Peripheral data size: 8-bit
    DMA1->S4CR &= ~(0b11 << 13); // Memory data size: 8-bit
}

void spi2_dma_write_no_increment(const uint16_t * data_buffer, uint32_t buffer_size)
{
    // Ensure DMA stream is disabled before configuring
    DMA1->S4CR &= ~(1U << 0);
    while (DMA1->S4CR & (1U << 0)); // Wait until EN bit is fully cleared

    // Wait for SPI to be not busy before changing configuration
    while((SPI2->SR & (1 << 7)));
    // Disable SPI to change data frame format
    SPI2->CR1 &= ~(1 << 6);
    // Set to 16-bit mode
    SPI2->CR1 |= (1 << 11); // Set DFF bit
    // Re-enable SPI
    SPI2->CR1 |= (1 << 6);

    // Configure DMA for 16-bit transfers and disable memory increment
    DMA1->S4CR &= ~(1U << 10);   // Disable Memory increment mode
    DMA1->S4CR |= (0b01 << 11);  // Peripheral data size: 16-bit (Half-word)
    DMA1->S4CR |= (0b01 << 13);  // Memory data size: 16-bit (Half-word)

    // Clear all interrupt flags for Stream 4 in High Interrupt Flag Clear Register
    // (Bits 0, 2, 3, 4, 5 correspond to Stream 4. Write 1 to clear)
    DMA1->HIFCR = (0b111101 << 0); 

    // Set addresses (S4PAR needs the address of the DR register)
    DMA1->S4PAR = (uint32_t)&SPI2->DR; 
    DMA1->S4M0AR = (uint32_t)data_buffer; 

    uint32_t remaining = buffer_size;

    GPIOB->ODR &= ~(1 << 12); // Pull CS pin low to prepare for transmission

    while (remaining > 0)
    {
        // Send up to 65535 items per DMA transaction
        uint16_t transfer_size = (remaining > 65535) ? 65535 : remaining;
        
        DMA1->S4NDTR = transfer_size; // Set number of data items
        DMA1->S4CR |= (1U << 0);      // Enable DMA stream
        
        // Wait for Transfer Complete (TCIF4 is bit 5 in HISR)
        while (!(DMA1->HISR & (1U << 5)));
        
        // Clear the Transfer Complete flag and others for Stream 4 so we can trigger again
        DMA1->HIFCR = (0b111101 << 0);
        
        remaining -= transfer_size;
    }

    // Wait for SPI busy flag to clear to ensure physical transmission is done
    while((SPI2->SR & (1 << 7)));

    GPIOB->ODR |= (1 << 12); // Set CS high to end transaction

    // Wait for SPI to be not busy before changing configuration
    while((SPI2->SR & (1 << 7)));
    // Disable SPI to change data frame format
    SPI2->CR1 &= ~(1 << 6);
    // Set back to 8-bit mode
    SPI2->CR1 &= ~(1 << 11); // Clear DFF bit
    // Re-enable SPI
    SPI2->CR1 |= (1 << 6);

    // Restore DMA configurations to defaults (8-bit, memory increment enabled)
    DMA1->S4CR |= (1U << 10);    // Enable Memory increment mode
    DMA1->S4CR &= ~(0b11 << 11); // Peripheral data size: 8-bit
    DMA1->S4CR &= ~(0b11 << 13); // Memory data size: 8-bit
}