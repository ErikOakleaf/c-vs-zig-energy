// BEEBS cubic benchmark
//
// Contributor: James Pallister <james.pallister@bristol.ac.uk>
//
// This file is part of the Bristol/Embecosm Embedded Benchmark Suite.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

// beebs benchmark ported to zig

const std = @import("std");
const io = @import("io");
const uart = @import("uart");
const math = std.math;

fn SolveCubic(a: f64, b: f64, c: f64, d: f64, solutions: *i32, x: []f64) void {
    const a1: f64 = b / a;
    const a2: f64 = c / a;
    const a3: f64 = d / a;

    const Q: f64 = (a1 * a1 - 3.0 * a2) / 9.0;
    const R: f64 = (2.0 * a1 * a1 * a1 - 9.0 * a1 * a2 + 27.0 * a3) / 54.0;
    const R2_Q3: f64 = R * R - Q * Q * Q;

    if (R2_Q3 <= 0) {
        solutions.* = 3;
        const theta: f64 = math.acos(R / math.sqrt(Q * Q * Q));
        x[0] = -2.0 * math.sqrt(Q) * math.cos(theta / 3.0) - a1 / 3.0;
        x[1] = -2.0 * math.sqrt(Q) * math.cos((theta + 2.0 * math.pi) / 3.0) - a1 / 3.0;
        x[2] = -2.0 * math.sqrt(Q) * math.cos((theta + 4.0 * math.pi) / 3.0) - a1 / 3.0;
    } else {
        solutions.* = 1;
        x[0] = math.pow(f64, math.sqrt(R2_Q3) + @abs(R), 1.0 / 3.0);
        x[0] += Q / x[0];
        x[0] *= if (R < 0.0) 1.0 else -1.0;
        x[0] -= a1 / 3.0;
    }
}

export fn main() linksection(".main") void {
    uart.resetIOBank0();
    io.gpioPin9Fsel();

    io.timerInit();
    const initTime: u64 = io.readTime();

    var dummyArray: [48]f64 = undefined;
    const dummySink: *volatile [48]f64 = &dummyArray;

    const a1: f64 = 1.0;
    const b1: f64 = -10.5;
    const c1: f64 = 32.0;
    const d1: f64 = -30.0;

    const a2: f64 = 1.0;
    const b2: f64 = -4.5;
    const c2: f64 = 17.0;
    const d2: f64 = -30.0;

    const a3: f64 = 1.0;
    const b3: f64 = -3.5;
    const c3: f64 = 22.0;
    const d3: f64 = -31.0;

    const a4: f64 = 1.0;
    const b4: f64 = -13.7;
    const c4: f64 = 1.0;
    const d4: f64 = -35.0;

    var solutions: i32 = 0;

    var output: [48]f64 = undefined;

    const amountTests: u32 = 25;

    io.gpioPin9High();

    for (0..amountTests) |_| {
        // solve some cubic functions
        // should get 3 solutions: 2, 6 & 2.5
        SolveCubic(a1, b1, c1, d1, &solutions, &output);
        for (0..48) |x| {
            dummySink.*[x] = output[x];
        }
        // should get 1 solution: 2.5
        SolveCubic(a2, b2, c2, d2, &solutions, &output);
        for (0..48) |x| {
            dummySink.*[x] = output[x];
        }
        SolveCubic(a3, b3, c3, d3, &solutions, &output);
        for (0..48) |x| {
            dummySink.*[x] = output[x];
        }
        SolveCubic(a4, b4, c4, d4, &solutions, &output);
        for (0..48) |x| {
            dummySink.*[x] = output[x];
        }
        // Now solve some random equations
        var inner_a1: f64 = 1;
        while (inner_a1 < 3) : (inner_a1 += 1) {
            var inner_b1: f64 = 10;
            while (inner_b1 > 8) : (inner_b1 -= 1) {
                var inner_c1: f64 = 5;
                while (inner_c1 < 6) : (inner_c1 += 0.5) {
                    var inner_d1: f64 = -1;
                    while (inner_d1 > -3) : (inner_d1 -= 1) {
                        SolveCubic(inner_a1, inner_b1, inner_c1, inner_d1, &solutions, &output);
                        for (0..48) |x| {
                            dummySink.*[x] = output[x];
                        }
                    }
                }
            }
        }
    }

    io.gpioPin9Low();

    const finishTime: u64 = io.readTime() - initTime;

    uart.uart0Init();
    uart.uartSendString("\r\nZig cubic root solver : ");
    uart.uartSendU32(amountTests);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(finishTime));
    uart.uartSendString(" microseconds");
}
