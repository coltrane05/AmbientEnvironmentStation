#include "ili9341.h"
#include "gpio.h"
#include "spi.h"
#include "systick.h"

static const uint8_t ili9341_init_sequence[] = {
    0x01, 0xFF, 150, // Software Reset

    // Power Control A and B, and Timing Controls
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xEA, 2, 0x00, 0x00,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xF7, 1, 0x20,

    // Power Control 1 and 2
    0xC0, 1, 0x23,
    0xC1, 1, 0x10,

    // VCOM Control 1 and 2
    0xC5, 2, 0x3E, 0x28,
    0xC7, 1, 0x86,

    // Memory Access Control (MADCTL) - Orientation and Color Order
    0x36, 1, 0x48,

    // Pixel Format Set - 16-bit RGB565
    0x3A, 1, 0x55,

    // Frame Rate and Display Function Control
    0xB1, 2, 0x00, 0x18,
    0xB6, 3, 0x08, 0x82, 0x27,
    0xF2, 1, 0x00,
    0x26, 1, 0x01,

    // Positive Gamma Correction
    0xE0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    // Negative Gamma Correction
    0xE1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,

    //Sleep Out
    0x11, 0xFF, 120,

    // Display ON
    0x29, 0,

    // End of Sequence
    0x00
};

void ili9341_init(void)
{
    uint32_t current_num_data_bytes = 0;
    uint32_t sequence_idx = 0;
    while (ili9341_init_sequence[sequence_idx] != 0x00)
    {
        uint8_t command = ili9341_init_sequence[sequence_idx];
        ili9341_send_command(command);
        sequence_idx++;

        if (ili9341_init_sequence[sequence_idx] == 0xFF)
        {
            sequence_idx++;
            delay(ili9341_init_sequence[sequence_idx]);
            sequence_idx++; 
        }
        else 
        {
            current_num_data_bytes = ili9341_init_sequence[sequence_idx];
            sequence_idx++;

            if (current_num_data_bytes > 0)
            {
                uint8_t current_data_chunk[15]; // Fixed maximum size to avoid Variable Length Array
                for (uint32_t i = 0; i < current_num_data_bytes; i++)
                {
                    current_data_chunk[i] = ili9341_init_sequence[sequence_idx];
                    sequence_idx++;
                }
                ili9341_send_data(current_data_chunk, current_num_data_bytes);
            }
        }
    }
}

void ili9341_send_command(uint8_t command)
{
    GPIOB->ODR &= ~(1 << 10); // DC pin low for command
    spi2_write(&command, 1);
}
void ili9341_send_data(uint8_t * data_buffer, uint32_t buffer_size)
{
    GPIOB->ODR |= (1 << 10); // DC pin high   
    spi2_write(data_buffer, buffer_size);
}

void ili9341_reset(void) 
{
    GPIOB->ODR &= ~(1 << 8); // pull reset pin low
    delay(25); // wait for 25 ms.
    GPIOB->ODR |= (1 << 8); // pull reset pin high
    delay(135); // wait for 135 ms.
}

void ili9341_set_address_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t column_data_buffer[4];
    uint8_t page_data_buffer[4];

    column_data_buffer[0] = (x1 >> 8);
    column_data_buffer[1] = (uint8_t)(x1 & 0x00FF);
    column_data_buffer[2] = (x2 >> 8);
    column_data_buffer[3] = (uint8_t)(x2 & 0x00FF);

    page_data_buffer[0] = (y1 >> 8);
    page_data_buffer[1] = (uint8_t)(y1 & 0x00FF);
    page_data_buffer[2] = (y2 >> 8);
    page_data_buffer[3] = (uint8_t)(y2 & 0x00FF);

    ili9341_send_command(0x2A);
    ili9341_send_data(column_data_buffer, 4);

    ili9341_send_command(0x2B);
    ili9341_send_data(page_data_buffer, 4);
}

void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    ili9341_set_address_window(x, y, x, y);

    uint8_t color_buffer[2];
    color_buffer[0] = (color >> 8);
    color_buffer[1] = (uint8_t)(color & 0x00FF);

    ili9341_send_command(0x2C); // Memory Write command
    ili9341_send_data(color_buffer, 2);
}

void ili9341_fill_screen(uint16_t color) {
    ili9341_set_address_window(0, 0, 239, 319);
    ili9341_send_command(0x2C);

    uint8_t color_upper = (color >> 8);
    uint8_t color_lower = (uint8_t)(color & 0x00FF);
    for (uint32_t i = 0; i < (76800 * 2); i = i + 2) {
        ili9341_send_data(&color_upper, 1);
        ili9341_send_data(&color_lower, 1);
    }
}

  