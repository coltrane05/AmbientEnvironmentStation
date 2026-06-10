#include "usart.h"
#include "register_macros.h"


static volatile RingBuffer_t ring_buffer = { .head = 0, .tail = 0 };

void usart2_print(const char * str) {
    while(*str) {
        uint32_t next_head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;

        while (next_head == ring_buffer.tail) {
            SET_BIT(USART2->CR1, 7);
        }

        ring_buffer.data[ring_buffer.head] = *str;
        ring_buffer.head = next_head;
        str++;
    }

    SET_BIT(USART2->CR1, 7);
}

void usart2_println(const char * str) {
    usart2_print(str);
    usart2_print("\r\n");
}

void usart2_handle_interrupt(void) {
    if (ring_buffer.head != ring_buffer.tail) {
        USART2->DR = ring_buffer.data[ring_buffer.tail];
        ring_buffer.tail = (ring_buffer.tail + 1) % RING_BUFFER_SIZE;
    }
    else {
        USART2->CR1 &= ~(1 << 7);
    }
}