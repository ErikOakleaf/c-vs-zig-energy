const io = @import("io");
const uart = @import("uart");

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

    var dummyArray: [5][5]i32 = undefined;
    const dummySink: *volatile [5][5]i32 = &dummyArray;

    var input1: [5][5]i32 = undefined;
    var input2: [5][5]i32 = undefined;

    var result: [5][5]i32 = undefined;

    const amountTest: usize = 25;
    for (0..amountTest) |_| {
        var i: i32 = 0;
        while (i < 10) : (i += 1) {
            for (0..5) |j| {
                input1[j][0] = i;
                input1[j][1] = i;
                input1[j][2] = i;
                input1[j][3] = i;
                input1[j][4] = i;

                input2[j][0] = i;
                input2[j][1] = i;
                input2[j][2] = i;
                input2[j][3] = i;
                input2[j][4] = i;
            }

            matirxMultiply5x5(&input1, &input2, &result);

            for (0..5) |x| {
                for (0..5) |y| {
                    dummySink.*[x][y] = result[x][y];
                }
            }
        }
    }

    const finishTime: u64 = io.readTime() - initTime;

    uart.uart0Init();
    uart.uartSendString("\r\nZig int matrix multiplication: ");
    uart.uartSendU32(amountTest);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(finishTime));
    uart.uartSendString(" microseconds");
}
