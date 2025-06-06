#include "io.h"
#include "uart.h"
#include <stdint.h>

// A compact byte-oriented AES-256 implementation.
// All lookup tables replaced with 'on the fly' calculations.
//
// Copyright (c) 2007-2011 Literatecode, http://www.literatecode.com
// Copyright (c) 2022 Ilia Levin (ilia@levin.sg)
//
// Other contributors: Hal Finney.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

// alterd implemntation of aes256 rijndael

typedef struct aes256_key_t {
    uint8_t raw[32];
} aes256_key_t;
typedef struct aes256_blk_t {
    uint8_t raw[16];
} aes256_blk_t;

typedef struct aes256_context_t {
    aes256_key_t key;
    aes256_key_t enckey;
    aes256_key_t deckey;
} aes256_context_t;

uint8_t rj_xtime(uint8_t x) {
    uint8_t y = 0xff & (x << 1);
    return (x & 0x80) ? (y ^ 0x1b) : y;
} // rj_xtime

#define rj_sbox(x) sbox[(x)]
#define rj_sbox_inv(x) sboxinv[(x)]

static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

static const uint8_t sboxinv[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

void subBytes(uint8_t *buf) {
    for (uint8_t i = 0; i < 16; i++) {
        buf[i] = rj_sbox(buf[i]);
    }
} // subBytes

void subBytes_inv(uint8_t *buf) {
    for (uint8_t i = 0; i < 16; i++) {
        buf[i] = rj_sbox_inv(buf[i]);
    }
} // subBytes_inv

void addRoundKey(uint8_t *buf, uint8_t *key) {
    for (uint8_t i = 0; i < 16; i++) {
        buf[i] ^= key[i];
    }
} // addRoundKey

void addRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk) {
    for (uint8_t i = 0; i < 16; i++) {
        buf[i] ^= (cpk[i] = key[i]);
        cpk[16 + i] = key[16 + i];
    }
} // addRoundKey_cpy

void shiftRows(uint8_t *buf) {
    register uint8_t i, j; // to make it potentially parallelable :)

    i = buf[1];
    buf[1] = buf[5];
    buf[5] = buf[9];
    buf[9] = buf[13];
    buf[13] = i;

    i = buf[10];
    buf[10] = buf[2];
    buf[2] = i;

    j = buf[3];
    buf[3] = buf[15];
    buf[15] = buf[11];
    buf[11] = buf[7];
    buf[7] = j;

    j = buf[14];
    buf[14] = buf[6];
    buf[6] = j;
} // shiftRows

void shiftRows_inv(uint8_t *buf) {
    register uint8_t i, j; // similar to shiftRows :)

    i = buf[1];
    buf[1] = buf[13];
    buf[13] = buf[9];
    buf[9] = buf[5];
    buf[5] = i;

    i = buf[2];
    buf[2] = buf[10];
    buf[10] = i;

    j = buf[3];
    buf[3] = buf[7];
    buf[7] = buf[11];
    buf[11] = buf[15];
    buf[15] = j;

    j = buf[6];
    buf[6] = buf[14];
    buf[14] = j;
} // shiftRows_inv

void mixColumns(uint8_t *buf) {
    register uint8_t a, b, c, d, e;

    for (uint8_t i = 0; i < 16; i += 4) {
        a = buf[i];
        b = buf[i + 1];
        c = buf[i + 2];
        d = buf[i + 3];
        e = a ^ b ^ c ^ d;
        buf[i] ^= e ^ rj_xtime(a ^ b);
        buf[i + 1] ^= e ^ rj_xtime(b ^ c);
        buf[i + 2] ^= e ^ rj_xtime(c ^ d);
        buf[i + 3] ^= e ^ rj_xtime(d ^ a);
    }
} // mixColumns

void mixColumns_inv(uint8_t *buf) {
    register uint8_t a, b, c, d, e, x, y, z;

    for (uint8_t i = 0; i < 16; i += 4) {
        a = buf[i];
        b = buf[i + 1];
        c = buf[i + 2];
        d = buf[i + 3];
        e = a ^ b ^ c ^ d;
        z = rj_xtime(e);
        x = e ^ rj_xtime(rj_xtime(z ^ a ^ c));
        y = e ^ rj_xtime(rj_xtime(z ^ b ^ d));
        buf[i] ^= x ^ rj_xtime(a ^ b);
        buf[i + 1] ^= y ^ rj_xtime(b ^ c);
        buf[i + 2] ^= x ^ rj_xtime(c ^ d);
        buf[i + 3] ^= y ^ rj_xtime(d ^ a);
    }
} // mixColumns_inv

void expandEncKey(uint8_t *k, uint8_t *rc) {
    k[0] ^= rj_sbox(k[29]) ^ (*rc);
    k[1] ^= rj_sbox(k[30]);
    k[2] ^= rj_sbox(k[31]);
    k[3] ^= rj_sbox(k[28]);
    *rc = rj_xtime(*rc);

    for (uint8_t i = 4; i < 16; i += 4) {
        k[i] ^= k[i - 4];
        k[i + 1] ^= k[i - 3];
        k[i + 2] ^= k[i - 2];
        k[i + 3] ^= k[i - 1];
    }

    k[16] ^= rj_sbox(k[12]);
    k[17] ^= rj_sbox(k[13]);
    k[18] ^= rj_sbox(k[14]);
    k[19] ^= rj_sbox(k[15]);

    for (uint8_t i = 20; i < 32; i += 4) {
        k[i] ^= k[i - 4];
        k[i + 1] ^= k[i - 3];
        k[i + 2] ^= k[i - 2];
        k[i + 3] ^= k[i - 1];
    }
} // expandEncKey

void expandDecKey(uint8_t *k, uint8_t *rc) {
    for (uint8_t i = 28; i > 16; i -= 4) {
        k[i + 0] ^= k[i - 4];
        k[i + 1] ^= k[i - 3];
        k[i + 2] ^= k[i - 2];
        k[i + 3] ^= k[i - 1];
    }

    k[16] ^= rj_sbox(k[12]);
    k[17] ^= rj_sbox(k[13]);
    k[18] ^= rj_sbox(k[14]);
    k[19] ^= rj_sbox(k[15]);

    for (uint8_t i = 12; i > 0; i -= 4) {
        k[i + 0] ^= k[i - 4];
        k[i + 1] ^= k[i - 3];
        k[i + 2] ^= k[i - 2];
        k[i + 3] ^= k[i - 1];
    }

    *rc = (((*rc) >> 1) ^ (((*rc) & 1) ? 0x8d : 0));
    k[0] ^= rj_sbox(k[29]) ^ (*rc);
    k[1] ^= rj_sbox(k[30]);
    k[2] ^= rj_sbox(k[31]);
    k[3] ^= rj_sbox(k[28]);
} // expandDecKey

void aes256_init(aes256_context_t *ctx, aes256_key_t *key) {
    ctx->enckey = ctx->deckey = *key;

    for (uint8_t i = 0, rcon = 1; i < 7; i++) {
        expandEncKey(ctx->deckey.raw, &rcon);
    }

} // aes256_init

void aes256_done(aes256_context_t *ctx) {
    const aes256_key_t zero = {0};

    ctx->key = ctx->enckey = ctx->deckey = zero;

} // aes256_done

void aes256_encrypt_ecb(aes256_context_t *ctx, aes256_blk_t *buf) {
    uint8_t rcon = 1;
    addRoundKey_cpy(buf->raw, ctx->enckey.raw, ctx->key.raw);

    for (uint8_t i = 1; i < 14; ++i) {
        subBytes(buf->raw);
        shiftRows(buf->raw);
        mixColumns(buf->raw);
        if (1 == (i & 1)) {
            addRoundKey(buf->raw, &ctx->key.raw[16]);
        } else {
            expandEncKey(ctx->key.raw, &rcon);
            addRoundKey(buf->raw, ctx->key.raw);
        }
    }

    subBytes(buf->raw);
    shiftRows(buf->raw);
    expandEncKey(ctx->key.raw, &rcon);
    addRoundKey(buf->raw, ctx->key.raw);
} // aes256_encrypt

void aes256_decrypt_ecb(aes256_context_t *ctx, aes256_blk_t *buf) {
    addRoundKey_cpy(buf->raw, ctx->deckey.raw, ctx->key.raw);
    shiftRows_inv(buf->raw);
    subBytes_inv(buf->raw);

    for (uint8_t i = 14, rcon = 0x80; --i;) {
        if (1 == (i & 1)) {
            expandDecKey(ctx->key.raw, &rcon);
            addRoundKey(buf->raw, &ctx->key.raw[16]);
        } else {
            addRoundKey(buf->raw, ctx->key.raw);
        }
        mixColumns_inv(buf->raw);
        shiftRows_inv(buf->raw);
        subBytes_inv(buf->raw);
    }

    addRoundKey(buf->raw, ctx->key.raw);
} // aes256_decrypt

void printHex128(uint8_t hex[16]) {
    // Array of hex digits
    const char hexChars[] = "0123456789abcdef";

    // Buffer for a single byte as hex (2 chars + null terminator)
    char hexByte[3];
    hexByte[2] = '\0';

    for (int i = 0; i < 16; i++) { // Loop 16 times for 128 bits (16 bytes)
        // Convert byte to two hex characters
        hexByte[0] = hexChars[(hex[i] >> 4) & 0xF]; // high nibble
        hexByte[1] = hexChars[hex[i] & 0xF];        // low nibble

        // Send the hex representation of this byte
        uartSendString(hexByte);
    }

    // Optional: send a newline at the end
    uartSendString("\r\n");
}

void main(void) __attribute__((section(".main")));
void main() {
    resetIOBank0();
    gpioPin9Fsel();

    timerInit();
    uint64_t initTime = readTime();

    // dummy value to make sure performing of the algorithm does not get optimized away
    uint8_t dummyArray[16];
    volatile uint8_t (*dummySink)[16] = &dummyArray;

    aes256_key_t key = {
        .raw = {97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 48, 49, 50, 51, 52, 53}};

    aes256_context_t ctx;

    aes256_blk_t block;

    uint32_t amountTests = 25;

	gpioPin9High();

    for (int i = 0; i < amountTests; i++) {
        for (int j = 100; j < 110; j++) {

            block.raw[0] = i;
            block.raw[1] = i;
            block.raw[2] = i;
            block.raw[3] = i;
            block.raw[4] = i;
            block.raw[5] = i;
            block.raw[6] = i;
            block.raw[7] = i;
            block.raw[8] = i;
            block.raw[9] = i;
            block.raw[10] = i;
            block.raw[11] = i;
            block.raw[12] = i;
            block.raw[13] = i;
            block.raw[14] = i;
            block.raw[15] = i;

            aes256_init(&ctx, &key);
            aes256_encrypt_ecb(&ctx, &block);
            aes256_decrypt_ecb(&ctx, &block);

            for (int x = 0; x < 16; x++) {
                (*dummySink)[x] = block.raw[x];
            }

            aes256_done(&ctx);
        }
    }

    gpioPin9Low();

    uint64_t finishTime = readTime() - initTime;

    uart0Init();
    uartSendString("\r\nc rijndeal(aes256) : ");
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(finishTime);
    uartSendString(" microseconds");
}
