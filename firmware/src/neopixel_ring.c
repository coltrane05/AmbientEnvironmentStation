#include <stdint.h>
#include "neopixel_ring.h"
#include "dma.h"
#include "timx.h"

// NeoPixel timing: 84MHz / (ARR+1) = 800kHz, 1.25us period
// Shorter 1-bit high time prevents Schmitt trigger drift across dense 1-bit clusters
#define NEOPIXEL_CCR_0  33  // 0-bit: ~0.39us high / ~0.86us low
#define NEOPIXEL_CCR_1  67  // 1-bit: ~0.77us high / ~0.48us low

#define NEOPIXEL_BUFFER_SIZE ((NUM_NEOPIXEL_LEDS * 24) + 80) // 24 LEDs * 24 bit + 300 bytes reset (375us)

uint16_t neopixel_color_buffer[NEOPIXEL_BUFFER_SIZE];

neopixel_color_t neopixel_ring_colors[] = {
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},
    {.r = 0, .g = 0, .b = 0},    
};

uint16_t color_cycle_steps[NUM_NEOPIXEL_LEDS];

// Private function declarations
void tim3_init (void);
void dma1_stream7_channel3_init (void);
void set_neopixel_buffer_color (neopixel_color_t color, uint8_t pos);

void tim3_init(void)
{
    TIM3->PSC = 0; // no prescaler
    TIM3->ARR = 104; // 84MHz / 105 = 800kHz, 1.25us nominal WS2812B period
    TIM3->CCMR2 &= ~(0b111 << 4); // Clear output compare mode
    TIM3->CCMR2 |= (0b110 << 4); // Set output compare mode to pwm 1
    TIM3->CCMR2 |= (1U << 3);    // Enable CCR3 preload - prevents mid-cycle glitch
                                  // on 0→1 transitions. First cycle is a harmless
                                  // LOW extension of the reset period.
    TIM3->CCER |= (1U << 8); // Enable Capture/Compare 3 output
    TIM3->DIER |= (1U << 11); // Enable DMA request on Capture/Compare 3
}

void dma1_stream7_channel3_init (void)
{
    DMA1->S7CR &= ~(1U << 0); // Ensure DMA stream is disabled before configuration
    DMA1->S7CR |= (0b101 << 25); // Set channel to 5 for TIM3 Channel 3
    DMA1->S7PAR = (uint32_t)&(TIM3->CCR3); // Set peripheral address to TIM3 channel 3 CCR 
    DMA1->S7M0AR = (uint32_t)neopixel_color_buffer;
    DMA1->S7NDTR = NEOPIXEL_BUFFER_SIZE; // Set number of data items to transfer
    DMA1->S7CR |= (0b01 << 6); // Set data transfer direction to Memory-to-Peripheral
    DMA1->S7CR |= (1U << 10); // Memory increment mode
    DMA1->S7CR &= ~(0b11 << 13); // Clear MSIZE bits 14:13
    DMA1->S7CR |= (0b01 << 13);  // 16-bit memory data size
    DMA1->S7CR &= ~(0b11 << 11); // Clear PSIZE bits 12:11
    DMA1->S7CR |= (0b01 << 11);  // 16-bit peripheral data size (TIM3 CCR3)
    DMA1->S7CR |= (1U << 4); // Enable transfer complete interrupt
}

void neopixel_init(void)
{
    tim3_init(); // Initialize TIM3 for PWM output

    dma1_stream7_channel3_init(); // Initialize DMA for neopixel data transfer

    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        color_cycle_steps[i] = (1530 / NUM_NEOPIXEL_LEDS) * i;
    }
}

void set_neopixel_color(neopixel_color_t color, uint8_t pos)
{
    neopixel_ring_colors[pos] = color;
}

void set_neopixel_buffer_color(neopixel_color_t color, uint8_t pos)
{
    // NeoPixels expect MSB-first bit order: G7,G6,...,G0, R7,...,R0, B7,...,B0
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t bit = 7 - i; // MSB-first: i=0 → bit 7, i=7 → bit 0

        if ((color.g >> bit) & 1U)
        {
            neopixel_color_buffer[(24 * pos) + i] = NEOPIXEL_CCR_1;
        }
        else
        {
            neopixel_color_buffer[(24 * pos) + i] = NEOPIXEL_CCR_0;
        }

        if ((color.r >> bit) & 1U)
        {
            neopixel_color_buffer[(24 * pos) + (i + 8)] = NEOPIXEL_CCR_1;
        }
        else
        {
            neopixel_color_buffer[(24 * pos) + (i + 8)] = NEOPIXEL_CCR_0;
        }
        
        if ((color.b >> bit) & 1U)
        {
            neopixel_color_buffer[(24 * pos) + (i + 16)] = NEOPIXEL_CCR_1;
        }
        else
        {
            neopixel_color_buffer[(24 * pos) + (i + 16)] = NEOPIXEL_CCR_0;
        }
    }
}

void generate_neopixel_color_buffer(void) {
    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        set_neopixel_buffer_color(neopixel_ring_colors[i], i);
    }

    for (uint8_t i = 0; i < 80; i++)
    {
        neopixel_color_buffer[(NUM_NEOPIXEL_LEDS * 24) + i] = 0;
    }
}

void send_neopixel_data(void)
{
    generate_neopixel_color_buffer();
    
    DMA1->S7CR &= ~(1U << 0);    // Disable DMA stream
    DMA1->S7NDTR = NEOPIXEL_BUFFER_SIZE; // Reload transfer count
    DMA1->S7M0AR = (uint32_t)neopixel_color_buffer; // Reset memory address
    DMA1->S7CR |= (1U << 0);     // Re-enable DMA stream
    TIM3->CNT = 0;               // Reset counter to 0 before starting
    TIM3->CR1 |= (1U << 0);      // Start timer 3 to trigger the DMA transfer.
}

volatile uint32_t neopixel_transfer_count = 0;

void neopixel_dma_handle_interrupt(void)
{
    neopixel_transfer_count++;
    TIM3->CR1 &= ~(1U << 0); // Disable TIM3 Counter
    DMA1->S7CR &= ~(1U << 0); // Disable DMA Stream

    // Clear all interrupt flags for Stream 7. This includes Transfer Complete,
    // Half Transfer, Transfer Error, Direct Mode Error, and FIFO Error.
    DMA1->HIFCR = (1U << 27) | (1U << 26) | (1U << 25) | (1U << 24) | (1U << 22);
}

uint32_t get_neopixel_transfer_count(void)
{
    return neopixel_transfer_count;
}

void get_temperature_color(int32_t temp, int32_t min_temp, int32_t max_temp, uint8_t * r, uint8_t * g, uint8_t * b)
{
    if (max_temp <= min_temp)
    {
        *r = 255;
        *g = 0;
        *b = 0;
        return;
    }

    if (temp < min_temp)
    {
        temp = min_temp;
    }
    if (temp > max_temp)
    {
        temp = max_temp;
    }

    int32_t range = max_temp - min_temp;
    int32_t t = ((temp - min_temp) * 255) / range;

    if (t < 128)
    {
        *r = 0;
        *g = t * 2;
        *b = 255 - (t * 2);
    }
    else
    {
        *r = (t - 128) * 2;
        *g = 255 - ((t - 128) * 2);
        *b = 0;
    }
}

void get_humidity_color(uint32_t humidity, uint32_t min_humidity, uint32_t max_humidity, uint8_t * r, uint8_t * g, uint8_t * b)
{
    if (max_humidity <= min_humidity)
    {
        *r = 0;
        *g = 0;
        *b = 255;
        return;
    }

    if (humidity < min_humidity)
    {
        humidity = min_humidity;
    }
    if (humidity > max_humidity)
    {
        humidity = max_humidity;
    }

    uint32_t range = max_humidity - min_humidity;
    uint32_t h = ((humidity - min_humidity) * 255) / range;

    *r = 255 - h;
    *g = 255 - h;
    *b = 255;
}

void get_pressure_color(uint32_t pressure, uint32_t min_pressure, uint32_t max_pressure, uint8_t * r, uint8_t * g, uint8_t * b)
{
    if (max_pressure <= min_pressure)
    {
        *r = 255;
        *g = 127;
        *b = 0;
        return;
    }

    if (pressure < min_pressure)
    {
        pressure = min_pressure;
    }
    if (pressure > max_pressure)
    {
        pressure = max_pressure;
    }

    uint32_t range = max_pressure - min_pressure;
    uint32_t p = ((pressure - min_pressure) * 255) / range;

    *r = 128 + (p / 2);
    *g = p / 2;
    *b = 128 - (p / 2);
}

void set_rgb_animation_colors(void)
{
    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        uint16_t step = color_cycle_steps[i];

        if (step < 255)
        {
            neopixel_ring_colors[i].r = 255;
            neopixel_ring_colors[i].g = step;
            neopixel_ring_colors[i].b = 0;
        }
        else if (step < 510)
        {
            neopixel_ring_colors[i].r = 255 - (step - 255);
            neopixel_ring_colors[i].g = 255;
            neopixel_ring_colors[i].b = 0;
        }
        else if (step < 765)
        {
            neopixel_ring_colors[i].r = 0;
            neopixel_ring_colors[i].g = 255;
            neopixel_ring_colors[i].b = step - 510;
        }
        else if (step < 1020)
        {
            neopixel_ring_colors[i].r = 0;
            neopixel_ring_colors[i].g = 255 - (step - 765);
            neopixel_ring_colors[i].b = 255;
        }
        else if (step < 1275)
        {
            neopixel_ring_colors[i].r = step - 1020;
            neopixel_ring_colors[i].g = 0;
            neopixel_ring_colors[i].b = 255;
        }
        else if (step < 1530)
        {
            neopixel_ring_colors[i].r = 255;
            neopixel_ring_colors[i].g = 0;
            neopixel_ring_colors[i].b = 255 - (step - 1275);
        }
    }
}

void increment_rgb_animation_colors(void)
{
    for (uint8_t i = 0; i < NUM_NEOPIXEL_LEDS; i++)
    {
        if ((i & 1) == 0)
        {
            color_cycle_steps[i] = (color_cycle_steps[i] >= 1529) ? 0 : color_cycle_steps[i] + 1;
        }
        else
        {
            color_cycle_steps[i] = (color_cycle_steps[i] == 0) ? 1529 : color_cycle_steps[i] - 1;
        }
    }
}
