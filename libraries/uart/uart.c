#include "uart.h"

void enablePeriClock() {
    write32((volatile uint_32 *)(CLOCK_BASE + CLK_PERI_CTRL),
            (1 << 11) | (0x4 << 5));
}

void resetIOBank0() {
    write32((volatile uint_32 *)(RESETS_BASE + 0x3000), 1 << 5);
    while (((read32((volatile uint_32 *)(RESETS_BASE + 0x8))) & (1 << 5)) ==
           0) {
    } // wait until the reset done register is written
}

void resetUart0() {
    write32((volatile uint_32 *)(RESETS_BASE + 0x3000), 1 << 22);
    while (((read32((volatile uint_32 *)(RESETS_BASE + 0x8))) & (1 << 22)) ==
           0) {
    }
} // wait until the reset done register is written

void functionSelectUart() {
    write32((volatile uint_32 *)(IO_BANK0_BASE + GPIO0_CTRL), 2);
    write32((volatile uint_32 *)(IO_BANK0_BASE + GPIO1_CTRL), 2);
}

void setBaudRate() {
    write32((volatile uint_32 *)(UART0_BASE + 0x24), 6);
    write32((volatile uint_32 *)(UART0_BASE + 0x28), 33);
}

void enableFIFOs() {
    // set word length to 8 bits
    write32((volatile uint_32 *)(UART0_BASE + 0x2c), (0b11 << 5) | (1 << 4));
}

void enableTransmitReceive() {
    write32((volatile uint_32 *)(UART0_BASE + 0x30),
            (1 << 0) | (1 << 8) | (1 << 9));
}

void uart0Init() {
    enablePeriClock();
    resetIOBank0();
    resetUart0();
    functionSelectUart();
    setBaudRate();
    enableFIFOs();
    enableTransmitReceive();
}

void uartSend(char character) {
    // check that the FIFO is not full
    while ((read32((volatile uint_32 *)(UART0_BASE + 0x18)) & (1 << 5)) != 0) {
    }
    // but the data bits in the data memory register
    write32((volatile uint_32 *)(UART0_BASE + 0), (uint_32)character);
}

char uartReceive() {
    while ((read32((volatile uint_32 *)(UART0_BASE + 0x18)) & (1 << 4)) != 0) {
    }

    return (char)read32((volatile uint_32 *)(UART0_BASE + 0));
}
