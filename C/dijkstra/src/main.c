#define IO_BANK0_BASE 0x40014000
#define GPIO25_CTRL 0x0cc

typedef unsigned int uint_32;

void write32(volatile uint_32 *address, uint_32 value) {
    *address = value;
}

void delay(int cycles) {
    volatile int i = 0;
    while (i < cycles) {
        i++;
    }
}

void main(void) __attribute__((section(".main")));
void main() {
    write32((volatile uint_32 *)(IO_BANK0_BASE + GPIO25_CTRL), 5);
    write32((volatile uint_32 *)(0xd0000000 + 0x024), (1 << 25));

    while (1) {
        write32((volatile uint_32 *)(0xd0000000 + 0x014), (1 << 25));

        delay(100000);

        write32((volatile uint_32 *)(0xd0000000 + 0x018), (1 << 25));

        delay(100000);
    }
}
