#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

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

typedef enum {
    IDLE,
    SEND_ADDR_WRITE,
    SEND_READ_REG,
    REPEAT_START,
    SEND_ADDR_READ,
    BURST_READ,
    SINGLE_READ_SETUP,
    SINGLE_READ
} I2C_read_state_t;

typedef struct{
    I2C_read_state_t currState;
    uint8_t address;
    uint8_t reg;
    uint32_t num_bytes;
    uint8_t* buffer;
    uint32_t num_bytes_to_read;
} I2C_read_stateMachine_t;

void I2C1_master_write_register(uint8_t address, uint8_t reg, uint8_t data);
void I2C1_master_receive(uint8_t address, uint8_t reg, uint8_t num_bytes, uint8_t * buffer);
void I2C1_handle_interrupt(void);
void I2C1_handle_error_interrupt(void);
bool I2C_read_is_busy(void);


#endif