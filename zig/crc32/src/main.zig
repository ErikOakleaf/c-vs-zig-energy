const io = @import("io");
const uart = @import("uart");
const test_data = @import("test_data");
const POLYNOM = 0xEDB88320;

fn crc32(input: []u8, output: *u32) void {
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
    const initTime: u64 = io.readTime();

    var dummyValue: u32 = undefined;
    const dummySink: *volatile u32 = &dummyValue;

    var output: u32 = undefined;
    var input: [10]u8 = undefined;

    const amountTests: u32 = 25;
    for (0..amountTests) |_| {
        for (100..110) |i| {
            input[0] = @intCast(i);
            input[1] = @intCast(i);
            input[2] = @intCast(i);
            input[3] = @intCast(i);
            input[4] = @intCast(i);
            input[5] = @intCast(i);
            input[6] = @intCast(i);
            input[7] = @intCast(i);
            input[8] = @intCast(i);
            input[9] = @intCast(i);

            crc32(input[0..], &output);

            dummySink.* = output;
        }
    }

    const finishTime: u64 = io.readTime() - initTime;

    uart.uart0Init();
    uart.uartSendString("\r\nZig crc32: ");
    uart.uartSendU32(amountTests);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(finishTime));
    uart.uartSendString(" microseconds");
}
