#include "io.h"
#include <stdint.h>

void write32(volatile uint32_t *address, uint32_t value) {
    *address = value;
}

uint32_t read32(volatile uint32_t *address) {
    // get the value from a adress
    return (uint32_t)*address;
}

void timerInit() {
    write32((volatile uint32_t *)(RESETS_BASE + 0x3000), 1 << 21);
    while (((read32((volatile uint32_t *)(RESETS_BASE + 0x8))) & (1 << 21)) ==
           0) {
    }
} // wait until the reset done register is written

uint_64 readTime() {
    uint32_t low, high;

    // Read the high register, then low, then high again to check for rollover.
    do {
        high = read32((volatile uint32_t *)TIMER_TIME_HIGH);
        low = read32((volatile uint32_t *)TIMER_TIME_LOW);
    } while (high != read32((volatile uint32_t *)TIMER_TIME_HIGH));

    return ((uint_64)high << 32) | low;
}

uint32_t getRandom() {
    return (uint32_t)(read32((volatile uint32_t *)RANDOM_BIT) & 0b1);
}

void gpioPin9Fsel() {
    write32((volatile uint32_t *)(0x40014000+ 0x04c), 5);
    write32((volatile uint32_t *)(0xd0000000 + 0x024), (1 << 9));
}

void gpioPin9High() {
    write32((volatile uint32_t *)(0xd0000000 + 0x014), (1 << 9));
}

void gpioPin9Low() {
    write32((volatile uint32_t *)(0xd0000000 + 0x018), (1 << 9));
}
