#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t global_uint = 10;

int main() {
    uint8_t stack_uint = 12;
    
    uint8_t *ptr_uint = (uint8_t *)malloc(sizeof(uint8_t));

    const uint8_t const_uint = 11;

    printf("%p\n", (void*)&global_uint);
    printf("%p\n", (void*)&stack_uint);
    printf("%p\n", (void*)ptr_uint);
    printf("%p\n\n", (void*)&const_uint);

    printf("Memory maps are fun!!!!");
    return 0;
}