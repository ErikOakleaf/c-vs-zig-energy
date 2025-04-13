#include "io.h"
#include <stdint.h>
#define GPIO25_CTRL 0x0cc
#define RESETS_BASE 0x4000c000
#define IO_BANK0_BASE 0x40014000

void delay(int cycles) {
    volatile int i = 0;
    while (i < cycles) {
        i++;
    }
}

void main(void) __attribute__((section(".main")));
void main() {

    write32((volatile uint32_t *)(RESETS_BASE + 0x3000), 1 << 5);
    while (((read32((volatile uint32_t *)(RESETS_BASE + 0x8))) & (1 << 5)) ==
           0) {
    } // wait until the reset done register is written

    write32((volatile uint32_t *)(IO_BANK0_BASE + GPIO25_CTRL), 5);
    write32((volatile uint32_t *)(0xd0000000 + 0x024), (1 << 25));

    while (1) {
        write32((volatile uint32_t *)(0xd0000000 + 0x014), (1 << 25));
        delay(100000);
        write32((volatile uint32_t *)(0xd0000000 + 0x018), (1 << 25));
        delay(100000);
    }
}
