const io = @import("io");
const uart = @import("uart");

fn matirxMultiply5x5(matrix1: [5][5]i32, matrix2: [5][5]i32, result: *[5][5]i32) void {
    for (0..5) |i| {
        for (0..5) |j| {
            result[i][j].* = 0;
            for (0..5) |k| {
                result[i][j].* += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

export fn main() linksection(".main") void {
    io.timerInit();

    uart.uart0Init();
    uart.uartSendU32(ammountTest);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(io.readTime()));
    uart.uartSendString(" microseconds");
}
