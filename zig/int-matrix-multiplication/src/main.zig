const io = @import("io");
const uart = @import("uart");
const test_data = @import("test_data");

fn matirxMultiply5x5(matrix1: *const [5][5]i32, matrix2: *const [5][5]i32, result: *[5][5]i32) void {
    for (0..5) |i| {
        for (0..5) |j| {
            result.*[i][j] = 0;
            for (0..5) |k| {
                result.*[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

fn testChecksum(result: *const [5][5]i32, checksum: i32) void {
    var compare: i32 = 0;
    for (0..5) |i| {
        for (0..5) |j| {
            compare += result[j][i];
        }
    }
    if (!(compare == checksum)) {
        uart.uart0Init();
        uart.uartSendString("checksum error");
        uart.uartSendString("\r\n");
    }
}

export fn main() linksection(".main") void {
    io.timerInit();
    const initTime: u64 = io.readTime();

    const amountTest: usize = 500;

    var result: [5][5]i32 = undefined;

    for (0..amountTest) |_| {
        for (0..test_data.NUM_TEST_MATRICES) |j| {
            matirxMultiply5x5(&test_data.test_matrices[j].matrix1, &test_data.test_matrices[j].matrix2, &result);
            testChecksum(&result, test_data.test_matrices[j].result_checksum);
        }
    }

    uart.uart0Init();
    uart.uartSendU32(amountTest);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(io.readTime() - initTime));
    uart.uartSendString(" microseconds");
}
