const io = @import("io");
const RESETS_BASE: u32 = 0x4000c000;
const CLOCK_BASE: u32 = 0x40008000;
const CLK_PERI_CTRL: u32 = 0x48; // offset for stetting up periphiral clock
const IO_BANK0_BASE: u32 = 0x40014000;
const GPIO0_CTRL: u32 = 0x4;
const GPIO1_CTRL: u32 = 0xc;
const UART0_BASE: u32 = 0x40034000;

pub fn enablePeriClock() void {
    io.write32(@ptrFromInt(CLOCK_BASE + CLK_PERI_CTRL), (1 << 11) | (0x4 << 5));
}

pub fn resetIOBank0() void {
    io.write32(@ptrFromInt(RESETS_BASE + 0x3000), 1 << 5);
    while (io.read32(@ptrFromInt(RESETS_BASE + 0x8)) & 1 << 5 == 0) {}
}

pub fn resetUart0() void {
    io.write32(@ptrFromInt(RESETS_BASE + 0x3000), 1 << 22);
    while (io.read32(@ptrFromInt(RESETS_BASE + 0x8)) & 1 << 22 == 0) {}
}

pub fn functionSelectUart() void {
    io.write32(@ptrFromInt(IO_BANK0_BASE + GPIO0_CTRL), 2);
    io.write32(@ptrFromInt(IO_BANK0_BASE + GPIO1_CTRL), 2);
}

pub fn setBaudRate() void {
    io.write32(@ptrFromInt(UART0_BASE + 0x24), 6);
    io.write32(@ptrFromInt(UART0_BASE + 0x28), 33);
}

pub fn enableFIFOs() void {
    // set word length to 8 bits
    io.write32(@ptrFromInt(UART0_BASE + 0x2c), (0b11 << 5) | (1 << 4));
}

pub fn enableTransmitReceive() void {
    io.write32(@ptrFromInt(UART0_BASE + 0x30), (1 << 0) | (1 << 8) | (1 << 9));
}

pub fn uart0Init() void {
    enablePeriClock();
    resetIOBank0();
    resetUart0();
    functionSelectUart();
    setBaudRate();
    enableFIFOs();
    enableTransmitReceive();
}

pub fn uartSend(character: u8) void {
    // check that the FIFO is not full
    while ((io.read32(@ptrFromInt(UART0_BASE + 0x18)) & (1 << 5)) != 0) {}
    // but the data bits in the data memory register
    io.write32(@ptrFromInt(UART0_BASE + 0), @intCast(character));
}

pub fn uartReceive() u8 {
    while ((io.read32(@ptrFromInt(UART0_BASE + 0x18)) & (1 << 4)) != 0) {}

    return @intCast(io.read32(@ptrFromInt(UART0_BASE + 0)));
}
