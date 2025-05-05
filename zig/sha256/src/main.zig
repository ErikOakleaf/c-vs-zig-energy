const io = @import("io");
const uart = @import("uart");

// * Filename:   sha256.c
// * Author:     Brad Conte (brad AT bradconte.com)
// * Copyright:
// * Disclaimer: This code is presented "as is" without any guarantees.
// * Details:    Implementation of the SHA-256 hashing algorithm.
//               SHA-256 is one of the three algorithms in the SHA2
//               specification. The others, SHA-384 and SHA-512, are not
//               offered in this implementation.
//               Algorithm specification can be found here:
//                * http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
//               This implementation uses little endian byte order.

// alterd sha256 implementation ported to zig

pub const SHA256_CTX = struct {
    data: [64]u8,
    datalen: u32,
    bitlen: u64,
    state: [8]u32,
};

// inline functions

inline fn ROTRIGHT(a: u32, b: u32) u32 {
    return (a >> b) | (a << (32 - b));
}

inline fn CH(x: u32, y: u32, z: u32) u32 {
    return (((x) & (y)) ^ (~(x) & (z)));
}

inline fn MAJ(x: u32, y: u32, z: u32) u32 {
    return (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)));
}

inline fn EP0(x: u32) u32 {
    return (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22));
}

inline fn EP1(x: u32) u32 {
    return (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25));
}

inline fn SIG0(x: u32) u32 {
    return (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3));
}

inline fn SIG1(x: u32) u32 {
    return (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10));
}

const SHA256_BLOCK_SIZE = 32;

// /**************************** VARIABLES *****************************/
const k: [64]u32 = .{ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

fn sha256_transform(ctx: *SHA256_CTX, data: []const u8) void {
    var a: u32 = undefined;
    var b: u32 = undefined;
    var c: u32 = undefined;
    var d: u32 = undefined;
    var e: u32 = undefined;
    var f: u32 = undefined;
    var g: u32 = undefined;
    var h: u32 = undefined;
    var t1: u32 = undefined;
    var t2: u32 = undefined;
    var m: [64]u32 = undefined;

    var i: usize = 0;
    var j: usize = 0;
    while (i < 16) : ({
        i += 1;
        j += 4;
    }) {
        m[i] = (@as(u32, data[j]) << 24) | (@as(u32, data[j + 1]) << 16) | (@as(u32, data[j + 2]) << 8) | @as(u32, (data[j + 3]));
    }

    while (i < 64) : (i += 1) {
        m[i] = SIG1(m[i - 2]) +% m[i - 7] +% SIG0(m[i - 15]) +% m[i - 16];
    }

    a = ctx.*.state[0];
    b = ctx.*.state[1];
    c = ctx.*.state[2];
    d = ctx.*.state[3];
    e = ctx.*.state[4];
    f = ctx.*.state[5];
    g = ctx.*.state[6];
    h = ctx.*.state[7];

    i = 0;
    while (i < 64) : (i += 1) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx.*.state[0] += a;
    ctx.*.state[1] += b;
    ctx.*.state[2] += c;
    ctx.*.state[3] += d;
    ctx.*.state[4] += e;
    ctx.*.state[5] += f;
    ctx.*.state[6] += g;
    ctx.*.state[7] += h;
}

fn sha256_init(ctx: *SHA256_CTX) void {
    ctx.*.datalen = 0;
    ctx.*.bitlen = 0;
    ctx.*.state[0] = 0x6a09e667;
    ctx.*.state[1] = 0xbb67ae85;
    ctx.*.state[2] = 0x3c6ef372;
    ctx.*.state[3] = 0xa54ff53a;
    ctx.*.state[4] = 0x510e527f;
    ctx.*.state[5] = 0x9b05688c;
    ctx.*.state[6] = 0x1f83d9ab;
    ctx.*.state[7] = 0x5be0cd19;
}

fn sha256_update(ctx: *SHA256_CTX, data: []const u8) void {
    for (data) |datapoint| {
        ctx.*.data[ctx.*.datalen] = datapoint;
        ctx.*.datalen += 1;
        if (ctx.*.datalen == 64) {
            sha256_transform(ctx, ctx.*.data[0..]);
            ctx.*.bitlen += 512;
            ctx.*.datalen = 0;
        }
    }
}

fn sha256_final(ctx: *SHA256_CTX, hash: []u8) void {
    var i: usize = @intCast(ctx.*.datalen);

    // Pad whatever data is left in the buffer.
    if (ctx.*.datalen < 56) {
        ctx.*.data[i] = 0x80;
        i += 1;
        while (i < 56) : (i += 1)
            ctx.*.data[i] = 0x00;
    } else {
        ctx.*.data[i] = 0x80;
        i += 1;
        while (i < 64) : (i += 1)
            ctx.*.data[i] = 0x00;
        sha256_transform(ctx, ctx.*.data[0..]);
        i = 0;
        while (i < 56) : (i += 1) {
            ctx.*.data[i] = 0;
        }
    }

    ctx.*.bitlen += ctx.*.datalen * 8;
    ctx.*.data[63] = @intCast(ctx.*.bitlen);
    ctx.*.data[62] = @intCast(ctx.*.bitlen >> 8);
    ctx.*.data[61] = @intCast(ctx.*.bitlen >> 16);
    ctx.*.data[60] = @intCast(ctx.*.bitlen >> 24);
    ctx.*.data[59] = @intCast(ctx.*.bitlen >> 32);
    ctx.*.data[58] = @intCast(ctx.*.bitlen >> 40);
    ctx.*.data[57] = @intCast(ctx.*.bitlen >> 48);
    ctx.*.data[56] = @intCast(ctx.*.bitlen >> 56);
    sha256_transform(ctx, ctx.*.data[0..]);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    i = 0;
    while (i < 4) : (i += 1) {
        hash[i] = @intCast((ctx.*.state[0] >> @as(u5, @intCast(24 - i * 8))) & 0xff);
        hash[i + 4] = @intCast((ctx.*.state[1] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 8] = @intCast((ctx.*.state[2] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 12] = @intCast((ctx.*.state[3] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 16] = @intCast((ctx.*.state[4] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 20] = @intCast((ctx.*.state[5] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 24] = @intCast((ctx.*.state[6] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
        hash[i + 28] = @intCast((ctx.*.state[7] >> @as(u5, @intCast(24 - i * 8))) & 0x000000ff);
    }
}

fn printHash(hash: [32]u8) void {
    // Array of hex digits
    const hexChars = "0123456789abcdef"[0..];

    // Buffer for a single byte as hex (2 chars + null terminator)
    var hexByte: [2]u8 = undefined;

    for (hash) |hashByte| {
        // Convert byte to two hex characters
        hexByte[0] = hexChars[(hashByte >> 4) & 0x0f]; // high nibble
        hexByte[1] = hexChars[hashByte & 0x0f]; // low nibble

        // Send the hex representation of this byte
        uart.uartSendString(hexByte[0..]);
    }

    uart.uartSendString("\r\n");
}

export fn main() linksection(".main") void {
    uart.resetIOBank0();
    io.gpioPin9Fsel();

    io.timerInit();
    const initTime: u64 = io.readTime();

    // dummy value to make sure performing of the algorithm does not get optimized away
    var dummyArray: [32]u8 = undefined;
    const dummySink: *volatile [32]u8 = &dummyArray;

    var output: [32]u8 = undefined;
    var input: [5]u8 = undefined;
    var ctx: SHA256_CTX = undefined;

    const amountTests: u32 = 25;

    io.gpioPin9High();

    for (0..amountTests) |_| {
        for (100..110) |i| {
            input[0] = @intCast(i);
            input[1] = @intCast(i);
            input[2] = @intCast(i);
            input[3] = @intCast(i);
            input[4] = @intCast(i);

            sha256_init(&ctx);
            sha256_update(&ctx, input[0..]);
            sha256_final(&ctx, output[0..]);

            for (0..32) |x| {
                dummySink.*[x] = output[x];
            }
        }
    }

    io.gpioPin9Low();

    const finishTime: u64 = io.readTime() - initTime;

    uart.uart0Init();
    uart.uartSendString("\r\nZig sha256: ");
    uart.uartSendU32(amountTests);
    uart.uartSendString(" tests done, took: ");
    uart.uartSendU32(@intCast(finishTime));
    uart.uartSendString(" microseconds");
}
