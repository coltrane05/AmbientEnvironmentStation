#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C1_BASE 0x40005400UL

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
} Custom_I2C_t;

#define I2C1 ((Custom_I2C_t *) I2C1_BASE)

void I2C1_read(uint8_t address, uint8_t register);

#endif