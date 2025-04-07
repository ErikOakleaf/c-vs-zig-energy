/* BEEBS fir benchmark

   MDH WCET BENCHMARK SUITE.
   Copyright (C) 2014 Embecosm Limited and University of Bristol

   Contributor Pierre Langlois <pierre.langlois@embecosm.com>

   This file is part of the Bristol/Embecosm Embedded Benchmark Suite.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

// alterd finite impulse response filter source code from beebs

#include "io.h"
#include "uart.h"
#include <math.h>
#include <stdint.h>

#define OUTPUTSIZE 720

const uint32_t firInt[36] = {
    0xfffffffe, 0x1, 0x4, 0x3, 0xfffffffe, 0xfffffffc, 0x2, 0x7, 0x0, 0xfffffff7, 0xfffffffc, 0xc, 0xb, 0xfffffff2, 0xffffffe6, 0xf, 0x59, 0x7f, 0x59, 0xf, 0xffffffe6, 0xfffffff2, 0xb, 0xc, 0xfffffffc, 0xfffffff7, 0x0, 0x7, 0x2, 0xfffffffc, 0xfffffffe, 0x3, 0x4, 0x1, 0xfffffffe, 0};

const uint32_t inData[701] = {
    0x0, 0x0, 0x0, 0x0, 0x7f, 0x79, 0x72, 0x79, 0xd, 0xd, 0x0, 0x3, 0x5, 0x2, 0x3, 0x7f, 0x7f, 0x2, 0x7e, 0x0, 0x1, 0x7e, 0x1, 0x1, 0x7f, 0x0, 0x7f, 0x0, 0x2, 0x1, 0x1, 0x3, 0x1, 0x7f, 0x1, 0x0, 0x1, 0x1, 0x7d, 0x7b, 0x73, 0x6a, 0x77, 0x10, 0xe, 0x1, 0x5, 0x5, 0x5, 0x5, 0x7d, 0x0, 0x2, 0x7d, 0x0, 0x0, 0x7e, 0x1, 0x7e, 0x7f, 0x3, 0x7c, 0x7e, 0x6, 0x0, 0x7e, 0x3, 0x2, 0x7f, 0x7e, 0x7f, 0x2, 0x1, 0x7f, 0x1, 0x1, 0x0, 0x3, 0x0, 0x7f, 0x2, 0x0, 0x7f, 0x3, 0x1, 0x0, 0x0, 0x7d, 0x0, 0x3, 0x0, 0x7e, 0x7f, 0x2, 0x1, 0x7e, 0x0, 0x3, 0x7f, 0x7d, 0x1, 0x1, 0x1, 0x7f, 0x0, 0x5, 0x0, 0x7f, 0x2, 0x7e, 0x7f, 0x2, 0x1, 0x0, 0x7e, 0x0, 0x5, 0x0, 0x7f, 0x0, 0x7e, 0x1, 0x0, 0x7d, 0x1, 0x3, 0x7f, 0x0, 0x0, 0x7e, 0x2, 0x3, 0x7e, 0x7d, 0x72, 0x68, 0x71, 0x5, 0xc, 0x7, 0x2, 0x6, 0xd, 0x5, 0x7d, 0x3, 0x2, 0x7f, 0x0, 0x79, 0x7a, 0x3, 0x7e, 0x7d, 0x0, 0x7d, 0x2, 0x1, 0x7d, 0x8, 0x3, 0x7c, 0x6, 0x0, 0x7a, 0x6, 0x2, 0x7c, 0x3, 0x7e, 0x79, 0x6, 0x5, 0x74, 0x7f, 0xd, 0x7a, 0x78, 0x6, 0x5, 0x1, 0x0, 0x7d, 0x1, 0x4, 0x7c, 0x7f, 0x3, 0x7f, 0x5, 0x3, 0x7a, 0x6, 0xa, 0x76, 0x7c, 0xa, 0x7c, 0x7f, 0x6, 0x79, 0x3, 0xc, 0x75, 0x78, 0xa, 0x0, 0x79, 0x3, 0x7e, 0x7c, 0x6, 0x0, 0x79, 0x2, 0x7e, 0x7f, 0x6, 0x76, 0x7f, 0xd, 0x79, 0x7f, 0x6, 0x79, 0x6, 0x3, 0x71, 0x6, 0xa, 0x73, 0x7f, 0xa, 0x0, 0x7f, 0x7a, 0x7c, 0xa, 0x0, 0x75, 0x7f, 0xc, 0xa, 0x7c, 0x79, 0x9, 0xd, 0x7d, 0x7a, 0x5, 0xb, 0xa, 0x79, 0x7c, 0x16, 0x3, 0x72, 0xd, 0x7, 0x79, 0xc, 0x7, 0x7a, 0xb, 0x7, 0x7a, 0xa, 0x7, 0x79, 0xa, 0x5, 0x75, 0x6, 0x5, 0x79, 0x5, 0x6, 0x1, 0x6, 0x0, 0x7a, 0x2, 0x7, 0x3, 0x7d, 0x1, 0xa, 0x7, 0x2, 0x7f, 0x7f, 0x9, 0x7, 0x79, 0x79, 0x6, 0x8, 0x7d, 0x7a, 0x6, 0xc, 0x6, 0x7d, 0x7f, 0xd, 0x7, 0x79, 0x1, 0x6, 0x7f, 0x7f, 0x2, 0x3, 0x1, 0x7e, 0x1, 0x1, 0x7d, 0x1, 0x0, 0x7d, 0x6, 0x3, 0x7d, 0x5, 0x7, 0x7f, 0x7c, 0x1, 0x6, 0x6, 0x7c, 0x7a, 0x7, 0xa, 0x0, 0x78, 0x1, 0x8, 0x0, 0x79, 0x7a, 0x4, 0xa, 0x0, 0x78, 0x1, 0x6, 0x7a, 0x75, 0x7a, 0x0, 0x0, 0x79, 0x76, 0x7f, 0x7, 0x0, 0x7a, 0x7d, 0x2, 0x4, 0x7c, 0x7a, 0x2, 0x5, 0x7c, 0x7a, 0x7d, 0x7f, 0x0, 0x78, 0x75, 0x7f, 0x0, 0x79, 0x78, 0x79, 0x1, 0x3, 0x79, 0x79, 0x0, 0x0, 0x7f, 0x7f, 0x79, 0x7f, 0x2, 0x7a, 0x7c, 0x7d, 0x7c, 0x7f, 0x7d, 0x79, 0x7d, 0x0, 0x79, 0x7a, 0x7c, 0x7d, 0x0, 0x7d, 0x7d, 0x0, 0x0, 0x0, 0x0, 0x7d, 0x7d, 0x0, 0x7d, 0x7e, 0x0, 0x7e, 0x3, 0x3, 0x7d, 0x1, 0x5, 0x0, 0x7e, 0x7d, 0x7f, 0x3, 0x7d, 0x79, 0x1, 0x2, 0x7d, 0x7f, 0x1, 0x0, 0x0, 0x7f, 0x7f, 0x7e, 0x7f, 0x0, 0x7f, 0x0, 0x7c, 0x7d, 0x0, 0x79, 0x78, 0x7c, 0x7c, 0x7b, 0x7b, 0x7d, 0x7f, 0x0, 0x0, 0x7f, 0x0, 0x1, 0x2, 0x0, 0x7f, 0x0, 0x0, 0x0, 0x7f, 0x7e, 0x0, 0x0, 0x7f, 0x0, 0x2, 0x1, 0x2, 0x6, 0x5, 0x3, 0x6, 0x8, 0x5, 0x2, 0x1, 0x1, 0x3, 0x0, 0x7d, 0x7f, 0x0, 0x7f, 0x7e, 0x0, 0x2, 0x3, 0x2, 0x1, 0x2, 0x3, 0x1, 0x7c, 0x7d, 0x0, 0x0, 0x7e, 0x7c, 0x7f, 0x1, 0x0, 0x7e, 0x7c, 0x7f, 0x1, 0x0, 0x7e, 0x7f, 0x2, 0x3, 0x1, 0x0, 0x4, 0x6, 0x5, 0x6, 0x7, 0xa, 0xa, 0x4, 0x2, 0x5, 0x8, 0x9, 0x8, 0x7, 0xc, 0x14, 0x14, 0x10, 0xe, 0x14, 0x15, 0xf, 0x9, 0x7, 0x4, 0x7e, 0x76, 0x64, 0x41, 0x48, 0x7d, 0x6c, 0x3d, 0x67, 0x10, 0x6, 0x7d, 0x75, 0x7, 0x1d, 0x0, 0x6c, 0x2, 0x7d, 0x78, 0x77, 0x6f, 0x77, 0x1, 0x0, 0x2, 0x7, 0xa, 0x1c, 0x1c, 0x17, 0x23, 0x2f, 0x41, 0x43, 0x4f, 0x55, 0x58, 0x7e, 0x2, 0x4c, 0x10, 0x69, 0x2c, 0xd, 0x74, 0x2a, 0x74, 0x63, 0x29, 0x7c, 0x5e, 0x21, 0x35, 0x46, 0x24, 0x67, 0x35, 0x3c, 0x3c, 0x26, 0x26, 0x2f, 0x47, 0x64, 0x4, 0x13, 0x18, 0x27, 0x2b, 0x30, 0x1b, 0x7f, 0x78, 0x72, 0x68, 0x5c, 0x5a, 0x68, 0x7c, 0x3, 0xd, 0x26, 0x41, 0x51, 0x5a, 0x6a, 0x6c, 0x54, 0x78, 0x9, 0x45, 0x79, 0x1f, 0xb, 0x2e, 0x60, 0xb, 0x66, 0x7f, 0x68, 0x77, 0x4e, 0x46, 0x4a, 0x3b, 0x12, 0x5b, 0x37, 0x31, 0x21, 0xb, 0x12, 0x2e, 0x57, 0x7e, 0x19, 0x22, 0x2b, 0x3f, 0x3a, 0x25, 0xb, 0x79, 0x71, 0x68, 0x61, 0x5c, 0x66, 0x72, 0x6, 0x16, 0x29, 0x41, 0x5e, 0x6d, 0x66, 0x60, 0x6e, 0x17, 0x48, 0x36, 0x12, 0x17, 0x2f, 0x63, 0x78, 0x5c, 0x77, 0x6c, 0x75, 0x41, 0x49, 0x4f, 0x3b, 0xb, 0x54, 0x37, 0};

void firFilterInt(const uint32_t in[], uint32_t out[], uint32_t inputLength, const uint32_t coef[], uint32_t coefLength, uint32_t scale) {
    // set up for coefficients
    uint32_t coefStartIndex = 0;
    const uint32_t coefLength2 = (coefLength + 1) >> 1;

    // set up for input data
    const uint32_t inEndIndex = inputLength - 1;
    uint32_t inPtrIndex = coefLength2 - 1;

    // initial value of accumulation length for startup
    uint32_t accLength = coefLength2;

    for (int i = 0; i < inputLength; i++) {

        // set up pointer for accumulation
        uint32_t dataPtrIndex = inPtrIndex;
        uint32_t coefPtrIndex = coefStartIndex;

        // do accumulation and write result with scale factor

        uint32_t acc = (uint32_t)coef[coefPtrIndex++] * in[dataPtrIndex--];

        for (int j = 1; j < accLength; j++) {
            acc += (uint32_t)coef[coefPtrIndex++] * in[dataPtrIndex--];
        }
        out[i] = (uint32_t)(acc / scale);

        // check for end case

        if (inPtrIndex == inEndIndex) {
            accLength--;      // one shorter each time
            coefStartIndex++; // next coefficient each time
        }

        // if not at end, then check for startup, add to input pointer

        else {
            if (accLength < coefLength)
                accLength++;
            inPtrIndex++;
        }
    }
}

void main(void) __attribute__((section(".main")));
void main() {
    uint32_t output[OUTPUTSIZE];
    timerInit();
    uint64_t initTime = readTime();

    // variable to make sure that the test are actually done
    uint32_t sink;

    uint32_t amountTests = 500;

    for (int i = 0; i < amountTests; i++) {
        firFilterInt(inData, output, 700, firInt, 35, 285);

        sink = output[getRandom()];
    }

    uart0Init();
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(readTime() - initTime);
    uartSendString(" microseconds");
    uartSendString("\r\n sink value: ");


    uartSendU32(sink);
}
