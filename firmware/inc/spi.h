#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>

#define SPI2_BASE 0x40003800UL

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
} custom_spi_t;

#define SPI2 ((custom_spi_t *) SPI2_BASE)

typedef struct
{
    const volatile uint16_t * remaining_data_buffer;
    uint16_t transfer_size;
    uint16_t remaining;
    void (* callback)(void);
    uint16_t no_increment_value;
} dma_write_t;


void spi2_init (void);
void spi2_write (const uint8_t * data_buffer, uint32_t buffer_size);
void spi2_dma_write16(const uint16_t * data_buffer, uint32_t buffer_size);
void spi2_dma_write16_no_increment(const uint16_t * data_buffer, uint32_t buffer_size);
void spi2_dma_write16_non_blocking (const volatile uint16_t * data_buffer, uint32_t buffer_size, void (* callback)(void));
void spi2_dma_write16_no_increment_non_blocking (const volatile uint16_t * data_buffer, uint32_t buffer_size, void (* callback)(void));
void spi2_handle_dma_interrupt (void);
void spi2_process_callbacks (void);


#endif