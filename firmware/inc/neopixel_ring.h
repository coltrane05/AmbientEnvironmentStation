#ifndef NEOPIXEL_RING_H
#define NEOPIXEL_RING_H

#include <stdint.h>

#define NUM_NEOPIXEL_LEDS 24

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} neopixel_color_t;

void neopixel_init (void);
void set_neopixel_color (neopixel_color_t color, uint8_t pos);
void send_neopixel_data(void);
void neopixel_dma_handle_interrupt (void);
uint32_t get_neopixel_transfer_count (void);
void get_temperature_color (int32_t temp, int32_t min_temp, int32_t max_temp, uint8_t * r, uint8_t * g, uint8_t * b);

#endif