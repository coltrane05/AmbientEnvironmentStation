#include "i2c.h"
#include "register_macros.h"
#include "usart.h"
#include <stddef.h>

static volatile I2C_read_state_t read_state = IDLE;
static volatile I2C_read_stateMachine_t I2C_read_state_machine;

// Private function prototypes
void send_addr_write (void);
void send_read_reg (void);
void repeat_start (void);
void send_addr_read (void);
void burst_read (void);
void single_read_setup (void);
void single_read (void);

void I2C1_master_write_register (uint8_t address, uint8_t reg, uint8_t data)
{
    
    CLEAR_BIT(I2C1->CR2, 9); // Disable event interrupt
    CLEAR_BIT(I2C1->CR2, 8); // Disable error interrupt
    CLEAR_BIT(I2C1->CR2, 10); // Disable buffer interrupt

    SET_BIT(I2C1->CR1, 8); // Set START bit
    while (!(I2C1->SR1 & (1 << 0))) // Wait for Start Status
    {
    }

    I2C1->DR = (address << 1); // Send target device addr in Data register
    while (!(I2C1->SR1 & (1 << 1))) // Wait for ADDR to go high indicating ACK
    {
        if (I2C1->SR1 & (1 << 10))
        { // Check for AF (Acknowledge Failure)
            CLEAR_BIT(I2C1->SR1, 10); // Clear AF flag
            SET_BIT(I2C1->CR1, 9); // Set STOP bit
            return; // Exit to avoid infinite loop
        }
    }

    // This 3 line sequence clears the ADDR bit
    uint32_t dummy_read = I2C1->SR1; 
    dummy_read = I2C1->SR2;
    (void)dummy_read;

    I2C1->DR = reg; // Send target register address
    while (!(I2C1->SR1 & (1 << 7))) // Wait for TxE bit
    {
        if (I2C1->SR1 & (1 << 10))
        { // Check for AF
            CLEAR_BIT(I2C1->SR1, 10);
            SET_BIT(I2C1->CR1, 9);
            return;
        }
    }

    I2C1->DR = data; // Send actual data
    while (!(I2C1->SR1 & (1 << 2))) // Wait for BTF bit
    {
        if (I2C1->SR1 & (1 << 10))
        {
            CLEAR_BIT(I2C1->SR1, 10);
            SET_BIT(I2C1->CR1, 9);
            return;
        }
    }

    SET_BIT(I2C1->CR1, 9); // Set STOP bit
}

void I2C_read_state_machine_reset (void)
{
    I2C_read_state_machine.address = 0x00;
    I2C_read_state_machine.num_bytes = 0;
    I2C_read_state_machine.num_bytes_to_read = 0;
    I2C_read_state_machine.reg = 0x00;
    I2C_read_state_machine.currState = IDLE;
    I2C_read_state_machine.callback = NULL;
}

void I2C1_master_receive (uint8_t address, uint8_t reg, uint8_t num_bytes, uint8_t * buffer, void (* callback)(void))
{
    I2C_read_state_machine.address = address;
    I2C_read_state_machine.reg = reg;
    I2C_read_state_machine.num_bytes = num_bytes;
    I2C_read_state_machine.num_bytes_to_read = num_bytes;
    I2C_read_state_machine.buffer = buffer;
    I2C_read_state_machine.currState = SEND_ADDR_WRITE;
    I2C_read_state_machine.callback = callback;
    
    SET_BIT(I2C1->CR2, 9); // Enable event interrupt
    SET_BIT(I2C1->CR2, 8); // Enable error interrupt
    // Intentionally don't enable ITBUFEN here to prevent a TxE interrupt storm.
    // It gets enabled inside send_addr_read().
    
    SET_BIT(I2C1->CR1, 8); // Set start bit
}

bool I2C_read_is_busy (void)
{
    return I2C_read_state_machine.currState != IDLE;
}

void I2C1_handle_interrupt (void)
{
    switch (I2C_read_state_machine.currState)
    {
        case SEND_ADDR_WRITE:
            if (I2C1->SR1 & (1 << 0))
            {
                send_addr_write();
                I2C_read_state_machine.currState = SEND_READ_REG;
            }
            break;
        case SEND_READ_REG:
            if (I2C1->SR1 & (1 << 1))
            {
                send_read_reg();
                I2C_read_state_machine.currState = REPEAT_START;
            }
            break;
        case REPEAT_START:
            if (I2C1->SR1 & (1 << 2))
            {
                repeat_start();
                I2C_read_state_machine.currState = SEND_ADDR_READ;
            }
            break;
        case SEND_ADDR_READ:
            if (I2C1->SR1 & (1 << 0))
            {
                send_addr_read();
                if (I2C_read_state_machine.num_bytes > 1)
                {
                    I2C_read_state_machine.currState = BURST_READ;
                }
                else
                {
                    I2C_read_state_machine.currState = SINGLE_READ_SETUP;
                }
            }
            break;
        case BURST_READ:
            if (I2C1->SR1 & (1 << 1) || I2C1->SR1 & (1 << 6))
            {
                burst_read();
                if (I2C_read_state_machine.num_bytes_to_read == 0)
                {
                    void (* cb)(void) = I2C_read_state_machine.callback;
                    I2C_read_state_machine.currState = IDLE;
                    CLEAR_BIT(I2C1->CR2, 9); // Disable event interrupt
                    CLEAR_BIT(I2C1->CR2, 10); // Disable buffer interrupt
                    CLEAR_BIT(I2C1->CR2, 8); // Disable error interrupt
                    if (cb != NULL)
                    {
                        cb();
                    }
                }
            }
            break;
        case SINGLE_READ_SETUP:
            if (I2C1->SR1 & (1 << 1))
            {
                single_read_setup();
                I2C_read_state_machine.currState = SINGLE_READ;
            }
            break;
        case SINGLE_READ:
            if (I2C1->SR1 & (1 << 6))
            {
                single_read();
                void (* cb)(void) = I2C_read_state_machine.callback;
                I2C_read_state_machine.currState = IDLE;
                CLEAR_BIT(I2C1->CR2, 9); // Disable event interrupt
                CLEAR_BIT(I2C1->CR2, 10); // Disable buffer interrupt
                CLEAR_BIT(I2C1->CR2, 8); // Disable error interrupt
                if (cb != NULL)
                {
                    cb();
                }
            }
            break;
        default:
            break;
    }
}

void I2C1_handle_error_interrupt (void)
{
    if (I2C1->SR1 & (1 << 10))
    { // AF (Acknowledge Failure)
        CLEAR_BIT(I2C1->SR1, 10); // Clear AF flag
        SET_BIT(I2C1->CR1, 9); // Generate STOP condition
        I2C_read_state_machine.currState = IDLE; // Abort the state machine to unblock the main loop
        CLEAR_BIT(I2C1->CR2, 9); // Disable event interrupt
        CLEAR_BIT(I2C1->CR2, 10); // Disable buffer interrupt
        CLEAR_BIT(I2C1->CR2, 8); // Disable error interrupt
    }
}

void send_addr_write (void)
{
    I2C1->DR = (I2C_read_state_machine.address << 1);
}

void send_read_reg (void)
{
    uint32_t dummy_read = I2C1->SR1;
    dummy_read = I2C1->SR2;
    (void)dummy_read;

    CLEAR_BIT(I2C1->CR2, 10); // Disable ITBUFEN to prevent TxE interrupt storm while waiting for BTF
    I2C1->DR = I2C_read_state_machine.reg;
}

void repeat_start (void)
{
    SET_BIT(I2C1->CR1, 8); // Set START bit
}

void send_addr_read (void)
{
    if (I2C_read_state_machine.num_bytes > 1)
    {
        SET_BIT(I2C1->CR1, 10); // Set ACK bit to ACK received data
    }
    // Do not enable ITBUFEN here to prevent TxE interrupt storm. Enable it after ADDR is cleared.
    I2C1->DR = (I2C_read_state_machine.address << 1 | 1);
}

void burst_read (void)
{
    // If handling the ADDR event
    if (I2C1->SR1 & (1 << 1))
    {
        uint16_t dummy_read = I2C1->SR1;
        dummy_read = I2C1->SR2;
        (void)dummy_read;
        SET_BIT(I2C1->CR2, 10); // Re-enable ITBUFEN so RxNE triggers interrupts
        return; // Return and wait for the first RxNE interrupt
    }

    // If handling the RxNE event
    if (I2C1->SR1 & (1 << 6))
    {
        if (I2C_read_state_machine.num_bytes_to_read == 2)
        {
            CLEAR_BIT(I2C1->CR1, 10); // Clear ACK bit to NACK
            SET_BIT(I2C1->CR1, 9); // Before we read the Data register, set the STOP bit.
            I2C_read_state_machine.buffer[I2C_read_state_machine.num_bytes - I2C_read_state_machine.num_bytes_to_read] = I2C1->DR;
            I2C_read_state_machine.num_bytes_to_read--;
        }
        else
        {
            I2C_read_state_machine.buffer[I2C_read_state_machine.num_bytes - I2C_read_state_machine.num_bytes_to_read] = I2C1->DR;
            I2C_read_state_machine.num_bytes_to_read--;
        }
    }
}

void single_read_setup (void)
{
    CLEAR_BIT(I2C1->CR1, 10); // Clear ACK bit to NACK
    uint32_t dummy_read = I2C1->SR1;
    dummy_read = I2C1->SR2;
    (void)dummy_read;
    SET_BIT(I2C1->CR1, 9); // Set STOP bit 
    SET_BIT(I2C1->CR2, 10); // Re-enable ITBUFEN so RxNE triggers interrupts
}

void single_read (void)
{
    I2C_read_state_machine.buffer[0] = I2C1->DR;
    I2C_read_state_machine.num_bytes_to_read--;
}