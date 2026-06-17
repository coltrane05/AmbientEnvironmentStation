#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>

void ili9341_init (void);
void ili9341_send_command (uint8_t command);
void ili9341_send_data (uint8_t * data_buffer, uint32_t buffer_size);
void ili9341_reset (void);
void ili9341_set_address_window (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9341_draw_pixel (uint16_t x, uint16_t y, uint16_t color);
void ili9341_fill_screen (uint16_t color);
#endif