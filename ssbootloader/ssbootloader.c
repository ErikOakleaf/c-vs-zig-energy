#define XIP_CTRL_BASE 0x14000000
#define XIP_SSI_BASE 0x18000000
#define RESETS_BASE 0x4000c000
#define XOSC_BASE 0x40024000
#define CLOCK_BASE 0x40008000

#define CTRL0_XIP     \
    (0b11111 << 16) | \
        (0b11 << 8) // bitmask for setting xip ctrl0 to eeprom read mode and set
                    // it to transfer 32 bits each clock cycle

#define CTRL0_XIP_SPI (0b11 << 24) | (0b10 << 8) | (0b110 << 2)

typedef unsigned int uint_32;
typedef char uint_8;

void write32(volatile uint_32 *address, uint_32 value) {
    *address = value;
}

uint_32 read32(volatile uint_32 *address) {
    // get the value from a adress
    return (uint_32)*address;
}

void boot() {
    // init xpi to be able to copy from flash to sram later

    write32((volatile uint_32 *)XIP_CTRL_BASE,
            0);                                            // disable xip cache by writing 0 into the ctrl register
    write32((volatile uint_32 *)(XIP_SSI_BASE + 0x08), 0); // disable ssi
    write32((volatile uint_32 *)(XIP_SSI_BASE + 0x14),
            0x4); // set the baud rate for ssi
    write32((volatile uint_32 *)(XIP_SSI_BASE + 0x0), CTRL0_XIP);
    write32((volatile uint_32 *)(XIP_SSI_BASE + 0xF4), CTRL0_XIP_SPI);
    write32((volatile uint_32 *)(XIP_SSI_BASE + 0x08), 1); // enable ssi
    //
    // set system clock to external crystal oscilator

    write32((volatile uint_32 *)(XOSC_BASE), 0xaa0);    // set osc to 1_15MHZ
    write32((volatile uint_32 *)(XOSC_BASE + 0xc), 47); // XOSC delay

    write32((volatile uint_32 *)(XOSC_BASE + 0x2000), (0xfab << 12)); // enable xosc

    while (((read32((volatile uint_32 *)(XOSC_BASE + 0x4))) & (1 << 31)) == 0) {
    } // wait until clock is stable

    // switch the system clock over to crystal osc
    write32((volatile uint_32 *)(CLOCK_BASE + 0x30),
            0x2); // set ref clock as xosc
    write32((volatile uint_32 *)(CLOCK_BASE + 0x3c),
            0); // set sys clock as xosc

    // Copy prgoram to sram
    uint_8 *source =
        (uint_8 *)0x10000100;                   // Program location in flash memory.
                                                // offset by 256 bytes due to bootloader
    uint_8 *destination = (uint_8 *)0x20000000; // SRAM location in memory

    // copy program from flash to sram
    for (int i = 0; i < 256 * 1024; i++) {
        *destination = *source;
        destination++;
        source++;
    }

    // jump to the start of sram
    void *sram = (void *)0x20000000;
    goto *sram;
}
