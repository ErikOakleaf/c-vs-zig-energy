#include "io.h"

void write32(volatile uint_32 *address, uint_32 value) {
    *address = value;
}

uint_32 read32(volatile uint_32 *address) {
    // get the value from a adress
    return (uint_32)*address;
}

void timerInit() {
    write32((volatile uint_32 *)(RESETS_BASE + 0x3000), 1 << 21);
    while (((read32((volatile uint_32 *)(RESETS_BASE + 0x8))) & (1 << 21)) ==
           0) {
    }
} // wait until the reset done register is written

uint_64 readTime() {
    uint_32 low, high;

    // Read the high register, then low, then high again to check for rollover.
    do {
        high = read32((volatile uint_32 *)TIMER_TIME_HIGH);
        low = read32((volatile uint_32 *)TIMER_TIME_LOW);
    } while (high != read32((volatile uint_32 *)TIMER_TIME_HIGH));

    return ((uint_64)high << 32) | low;
}
