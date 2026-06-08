#include "i2c.h"
#include "register_macros.h"
#include "usart.h"
#include <stdio.h>

void I2C1_read(uint8_t address, uint8_t reg) {

    SET_BIT(I2C1->CR1, 8); // Set START bit
    while(!(I2C1->SR1 & (1 << 0))); // Wait for Start Status
    
    I2C1->DR = (address << 1); // Send target device addr in Data register
    while(!(I2C1->SR1 & (1 << 1))); // Wait for ADDR to go high indicating ACK

    // This 3 line sequence clears the ADDR bit
    uint32_t dummy_read = I2C1->SR1; 
    dummy_read = I2C1->SR2;
    (void)dummy_read;

    I2C1->DR = reg; // Send register we want read
    while(!(I2C1->SR1 & (1 << 2))); // Wait for BTF bit to go high confirming transmission
    
    SET_BIT(I2C1->CR1, 8); // Set START bit
    while(!(I2C1->SR1 & (1 << 0))); // Wait for start Status

    I2C1->DR = (address << 1 | 1); // Send Target device addr in Read Mode
    while(!(I2C1->SR1 & (1 << 1))); // Wait for ADDR to go high indicating ACK

    CLEAR_BIT(I2C1->CR1, 10); // Clear the ACK bit before clearing ADDR
    // Clear ADDR
    dummy_read = I2C1->SR1; 
    dummy_read = I2C1->SR2;
    SET_BIT(I2C1->CR1, 9); // Set STOP bit

    while(!(I2C1->SR1 & (1 << 6))); // Wait for RxNE flag indicating data reception
    uint8_t chip_id = I2C1->DR;

    char print_buffer[] = "BME280 Chip ID: 0x00";
    char hex_digits[] = "0123456789ABCDEF";
    print_buffer[18] = hex_digits[(chip_id >> 4) & 0x0F];
    print_buffer[19] = hex_digits[chip_id & 0x0F];

    usart2_println(print_buffer);
}
