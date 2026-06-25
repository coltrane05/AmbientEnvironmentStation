#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>
#include <stdbool.h>

#define ILI9341_COLORS {.RED = 0xF800, .GREEN = 0x07E0, .BLUE = 0x001F, .BLACK = 0x0000}

typedef struct 
{
    const uint16_t RED;
    const uint16_t GREEN;
    const uint16_t BLUE;
    const uint16_t BLACK;
} ili9341_colors;

typedef struct
{
    char * string;
    uint16_t x;
    uint16_t y;
    uint16_t color;
    uint16_t bg_color;
    uint16_t * current_offsets;
    uint16_t cursor;
    uint16_t baseline;
    uint16_t current_x;
    uint16_t current_y;
    uint16_t current_advance;
} ili9341_string_context_t;

typedef struct
{
    bool text_is_displayed;
    uint16_t min_x;
    uint16_t max_x;
    uint16_t min_y;
    uint16_t max_y;
} ili9341_text_bounds_t;

extern ili9341_text_bounds_t text_bounds;

void ili9341_init (void);
void ili9341_send_command (uint8_t command);
void ili9341_send_data (const uint8_t * data_buffer, uint32_t buffer_size);
void ili9341_reset (void);
void ili9341_set_address_window (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void ili9341_draw_pixel (uint16_t x, uint16_t y, uint16_t color);

void ili9341_fill_screen (uint16_t color);
void ili9341_fill_area (uint16_t color, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2);

bool color_change_is_ready (void);
void set_color_change_ready (void);
void reset_color_change_ready (void);

void ili9341_draw_icon (const volatile uint16_t * icon, uint16_t icon_buffer_size, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void ili9341_draw_character (char character, uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color);
void ili9341_draw_string (char * string, uint16_t x, uint16_t y, uint16_t color, uint16_t bg_color);

void ili9341_clear_text (void);
void screen_draw_busy_callback (void);
bool screen_draw_is_busy (void);

void set_clear_text_flag (void);
void reset_clear_text_flag (void);
bool clear_text_is_set (void);

void set_icon_flag (void);
void reset_icon_flag (void);
bool icon_flag_is_set (void);

void set_text_flag (void);
void reset_text_flag (void);
bool text_flag_is_set (void);

void set_change_state_forward_flag (void);
void reset_change_state_forward_flag (void);
bool change_state_forward_flag_is_set (void);

void set_change_state_backward_flag (void);
void reset_change_state_backward_flag (void);
bool change_state_backward_flag_is_set (void);

#endif