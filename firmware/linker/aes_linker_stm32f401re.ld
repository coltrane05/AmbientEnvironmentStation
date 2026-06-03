

/* Entry point for compiler */
ENTRY(Reset_Handler)

/* Memory map for SRAM and FLASH */
MEMORY {
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (xrw) : ORIGIN = 0X20000000, LENGTH = 96K
}

/* Calculate the address at which SRAM ends (end of stack) */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

/* Define memory sections */
SECTIONS {
    /* Vector table is always first thing in flash */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        . = ALIGN(4);

        *(.text*)
        *(.rodata*)

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;

        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } >SRAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;

        *(.bss*)

        . = ALIGN(4);
        _ebss = .;

        end = .;
        __end__ = .;
    } >SRAM
}
