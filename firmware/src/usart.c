#include "usart.h"
#include "register_macros.h"
#include "led_state_machine.h"
#include "bme280.h"
#include <stdbool.h>


// State variables for USART communication
static volatile RingBuffer_t ring_buffer = { .head = 0, .tail = 0 }; // Buffer for outgoing data
static ReadBuffer_t read_buffer = { .idx = 0 };                      // Buffer for accumulating incoming command strings
static char read_char[1];                                            // Buffer for single incoming character
bool read_char_ready = false;                                        // Flag to indicate a new char was received
bool read_buffer_ready = false;                                      // Flag to indicate a full command (string) was received

// Private function prototypes
static bool strings_match (const char * s1, const char * s2);

// Queues a null-terminated string into the transmission ring buffer 
// and enables the Transmit Data Register Empty (TXE) interrupt.
void usart2_print (const char * str)
{
    while (*str)
    {
        uint32_t next_head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;

        // If the ring buffer is full, wait for the TXE interrupt to transmit some data
        while (next_head == ring_buffer.tail)
        {
            SET_BIT(USART2->CR1, 7);
        }

        // Place the character into the buffer and advance the head pointer
        ring_buffer.data[ring_buffer.head] = *str;
        ring_buffer.head = next_head;
        str++;
    }

    // Enable TXE (Transmit Data Register Empty) interrupt to begin transmission
    SET_BIT(USART2->CR1, 7);
}

// Convenience function to print a string followed by a carriage return and line feed.
void usart2_println (const char * str)
{
    usart2_print(str);
    usart2_print("\r\n");
}

// Returns true if a complete string has been received (terminated by \r or \n).
bool read_buffer_is_ready (void)
{
    return read_buffer_ready;
}

// Parses the accumulated read_buffer and triggers actions based on recognized commands.
void print_read_buffer (void)
{
    usart2_println("");
    
    // Check for "read" command and trigger BME280 data collection
    if (strings_match(read_buffer.data, "read"))
    {
        set_check_BME();
    }

    // Clear the buffer state for the next command
    read_buffer.data[0] = '\0';
    read_buffer.idx = 0;
    
    SET_BIT(USART2->CR1, 5); // Re-enable RXNE interrupt if it was previously disabled
    read_buffer_ready = false;
}

// Returns true if a single character was received but didn't complete a command yet.
bool read_char_is_ready (void)
{
    return read_char_ready;
}

// Echoes a single received character back to the terminal (used for live typing feedback).
void print_read_char (void)
{
    usart2_print(read_char);
    read_char_ready = false;
}

// Main USART2 interrupt handler called by the NVIC.
// Handles Transmit Data Register Empty (TXE) and Read Data Register Not Empty (RXNE) events.
void usart2_handle_interrupt (void)
{
    // Check if the TXE (Transmit Data Register Empty) interrupt flag is set
    if (USART2->SR & (1 << 7))
    {
       // If there is data in the ring buffer, send the next byte
       if (ring_buffer.head != ring_buffer.tail)
       {
            USART2->DR = ring_buffer.data[ring_buffer.tail];
            ring_buffer.tail = (ring_buffer.tail + 1) % RING_BUFFER_SIZE;
        }
        else
        {
            // If the buffer is empty, disable the TXE interrupt
            USART2->CR1 &= ~(1 << 7);
        }
    }
    
    // Check if the RXNE (Read Data Register Not Empty) interrupt flag is set
    if (USART2->SR & (1 << 5))
    {
        read_char[0] = USART2->DR;
        if (read_char[0] == '\r' || read_char[0] == '\n')
        {
            CLEAR_BIT(USART2->CR1, 5); // Disable RXNE interrupt temporarily while processing
            
            // Null-terminate the string and flag that the buffer is ready
            read_buffer.data[read_buffer.idx] = '\0';
            read_buffer_ready = true;
        }
        else
        {
            // Store the incoming character and increment the index
            read_buffer.data[read_buffer.idx] = read_char[0];
            read_buffer.idx++;
            read_char_ready = true;
        }
    }
}

// Helper function to compare two strings without including the standard <string.h> library.
static bool strings_match (const char * s1, const char * s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}