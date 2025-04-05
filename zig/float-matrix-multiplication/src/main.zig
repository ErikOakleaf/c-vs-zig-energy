const io = @import("io");
const uart = @import("uart");
const test_data = @import("test_data");

fn matirxMultiply5x5(matrix1: *const [5][5]f64, matrix2: *const [5][5]f64, result: *[5][5]f64) void {
    for (0..5) |i| {
        for (0..5) |j| {
            result.*[i][j] = 0;
            for (0..5) |k| {
                result.*[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

fn generateChecksum(result: *const [5][5]f64, checksum: *f64) void {
    checksum.* = 0;
    for (0..5) |i| {
        for (0..5) |j| {
            checksum.* += result[j][i];
        }
    }
}

export fn main() linksection(".main") void {
    io.timerInit();
    uart.uart0Init();

    const amountTest: usize = 10;

    var result: [5][5]f64 = undefined;
    var checksum: f64 = undefined;

    for (0..amountTest) |_| {
        // for (0..test_data.NUM_TEST_MATRICES) |j| {
        for (0..1) |j| {
            matirxMultiply5x5(&test_data.test_matrices[j].matrix1, &test_data.test_matrices[j].matrix2, &result);
            generateChecksum(&result, &checksum);
            uart.uartSendU32(@intFromFloat(checksum));
        }
    }

    uart.uart0Init();
    uart.uartSendU32(amountTest);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(io.readTime()));
    uart.uartSendString(" microseconds");
}
