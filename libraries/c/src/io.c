#include "../include/io.h"

void write32(volatile uint_32 *address, uint_32 value) {
    *address = value;
}

uint_32 read32(volatile uint_32 *address) {
    // get the value from a adress
    return (uint_32)*address;
}
