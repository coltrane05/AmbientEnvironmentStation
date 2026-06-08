#include "usart.h"
#include "register_macros.h"


static volatile RingBuffer_t ring_buffer = { .head = 0, .tail = 0 };

void usart2_print(const char * str) {
    while(*str) {
        ring_buffer.data[ring_buffer.head] = *str;
        ring_buffer.head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;
        str++;
    }

    SET_BIT(USART2->CR1, 7);
}

void usart2_println(const char * str) {
    while(*str) {
        ring_buffer.data[ring_buffer.head] = *str;
        ring_buffer.head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;
        str++;
    }
    ring_buffer.data[ring_buffer.head] = '\r';
    ring_buffer.head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;
    ring_buffer.data[ring_buffer.head] = '\n';
    ring_buffer.head = (ring_buffer.head + 1) % RING_BUFFER_SIZE;
    ring_buffer.data[ring_buffer.head] = '\0';
    
    SET_BIT(USART2->CR1, 7);
}

void USART2_IRQHandler(void) {
    if((USART2->SR & (1 << 7))) {
        if (ring_buffer.head != ring_buffer.tail) {
            USART2->DR = ring_buffer.data[ring_buffer.tail];
            ring_buffer.tail = (ring_buffer.tail + 1) % RING_BUFFER_SIZE;
        }
        else {
            USART2->CR1 &= ~(1 << 7);
        }
    }
}