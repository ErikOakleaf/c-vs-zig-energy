const io = @import("io");
const uart = @import("uart");
const GPIO25_CTRL: u32 = 0x0cc;
const RESETS_BASE: u32 = 0x4000c000;
const IO_BANK0_BASE: u32 = 0x40014000;

export fn main() linksection(".main") void {
    uart.uart0Init();
    io.timerInit();

    io.write32(@ptrFromInt(IO_BANK0_BASE + GPIO25_CTRL), 5);
    io.write32(@ptrFromInt(0xd0000000 + 0x024), (1 << 25));
    io.write32(@ptrFromInt(0xd0000000 + 0x14), (1 << 25));

    uart.uartSendString("hello world");
    uart.uartSendString("let's try a long string here something something something that is a bit long and would give us something something something yes let's have a bit of a longer string yes yes yes longer string");
    uart.uartSendInt(5);

    uart.uartSendInt(12);
    // uart.uartSendInt(-91);

    uart.uartSendInt(5);

    const multTest: i32 = 5 * 5;
    uart.uartSendInt(multTest);

    const divTest: i32 = 10 / 5;
    uart.uartSendInt(divTest);

    uart.uartSendString("something something");
    uart.uartSendString("\n");

    // for (0..100) |_| {
    //     const initTime: u64 = io.readTime();
    //     uart.uartSendUInt64(io.readTime() - initTime);
    //     uart.uartSend('\n');
    // }
}
