const io = @import("io");
const uart = @import("uart");
const POLYNOM = 0xEDB88320;

fn crc32(input: []const u8, size: usize, output: *u32) void {
    var crc: u32 = 0xFFFFFFFF;

    for (0..size) |i| {
        crc ^= input[i];

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
    // const initTime: u64 = io.readTime();

    // const amountTests: u32 = 500;

    var output: u32 = undefined;

    crc32("h", 1, &output);

    uart.uart0Init();
    uart.uartSendU32(output);
    // uart.uartSendU32(amountTests);
    // uart.uartSendString(" tests done, took: ");
    // uart.uartSendU32(@intCast(io.readTime() - initTime));
    // uart.uartSendString(" microseconds");
}
