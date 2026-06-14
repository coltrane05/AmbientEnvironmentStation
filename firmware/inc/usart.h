#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

#define USART2_BASE 0x40004400UL

// A structure to represent the USART registers based on the STM32F4 series.
typedef struct
{
    volatile uint32_t SR;    // Status register (offset 0x00)
    volatile uint32_t DR;    // Data register (offset 0x04)
    volatile uint32_t BRR;   // Baud rate register (offset 0x08)
    volatile uint32_t CR1;   // Control register 1 (offset 0x0C)
    volatile uint32_t CR2;   // Control register 2 (offset 0x10)
    volatile uint32_t CR3;   // Control register 3 (offset 0x14)
    volatile uint32_t GTPR;  // Guard time and prescaler register (offset 0x18)
} Custom_USART_t;

#define USART2 ((Custom_USART_t *) USART2_BASE)

#define RING_BUFFER_SIZE 128
#define READ_BUFFER_SIZE 128

// A circular buffer structure used for non-blocking UART transmission.
typedef struct
{
    char data[RING_BUFFER_SIZE];
    uint16_t head; // Index where the next character will be written
    uint16_t tail; // Index of the next character to be transmitted
} RingBuffer_t;

// A linear buffer structure used for accumulating incoming UART characters into strings.
typedef struct
{
    char data[READ_BUFFER_SIZE];
    uint16_t idx; // Current index/length of the received string
} ReadBuffer_t;

// Transmission functions
void usart2_print (const char * str);
void usart2_println (const char * str);

// Receive buffer processing functions
bool read_buffer_is_ready (void); 
void print_read_buffer (void);

// Single character reception functions
bool read_char_is_ready (void);
void print_read_char (void);

// Interrupt handler
void usart2_handle_interrupt (void);

#endif