#ifndef ANIME_MATRIX_FONT_H
#define ANIME_MATRIX_FONT_H

#include <stdint.h>

typedef struct
{
    uint16_t bitmap_index;
    uint16_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    uint8_t ofs_x;
    uint8_t ofs_y;
} glyph_description_t;

typedef struct 
{
    uint8_t range_start;
    uint8_t range_length;
    uint8_t glyph_id_start;
    const uint8_t * glyph_id_ofs_list;
    uint8_t list_length;
} font_character_map_t;


uint16_t get_glyph_id (char caracter);
uint8_t * get_glyph_dimensions (char character);
uint16_t * get_glyph_offsets (char character);
uint16_t * generate_character_display_data (char character, uint16_t color, uint16_t bg_color);
#endif