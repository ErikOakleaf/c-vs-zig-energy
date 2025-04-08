const io = @import("io");
const uart = @import("uart");
const test_data = @import("test_data");
const POLYNOM = 0xEDB88320;

fn crc32(input: []const u8, output: *u32) void {
    var crc: u32 = 0xFFFFFFFF;

    for (input) |char| {
        crc ^= char;

        for (0..8) |_| {
            if ((crc & 1) != 0) {
                crc = (crc >> 1) ^ POLYNOM;
            } else {
                crc = crc >> 1;
            }
        }
    }

    output.* = crc ^ 0xFFFFFFFF;
}

export fn main() linksection(".main") void {
    io.timerInit();
    uart.uart0Init();
    const initTime: u64 = io.readTime();

    const amountTests: u32 = 1;

    var output: u32 = undefined;

    for (0..amountTests) |_| {
        for (test_data.crc32_data[0..]) |testCase| {
            crc32(testCase, &output);
            uart.uartSendU32(output);
            uart.uartSendString("\r\n");
        }
    }

    uart.uart0Init();
    uart.uartSendU32(output);
    uart.uartSendU32(amountTests);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(io.readTime() - initTime));
    uart.uartSendString(" microseconds");
}
