/* Forward discrete cosine transform

   Copyright (C) 2014 Embecosm Limited

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of BEEBS

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.  */

// alterd beebs benchmark

#include "io.h"
#include "uart.h"
#include <stdint.h>

// Other FDCT Parameters
#define CONST_BITS 13
#define PASS1_BITS 2

// Image block to be transformed:

int16_t block_ref[64] =
    {99, 104, 109, 113, 115, 115, 55, 55, 104, 111, 113, 116, 119, 56, 56, 56, 110, 115, 120, 119, 118, 56, 56, 56, 119, 121, 122, 120, 120, 59, 59, 59, 119, 120, 121, 122, 122, 55, 55, 55, 121, 121, 121, 121, 60, 57, 57, 57, 122, 122, 61, 63, 62, 57, 57, 57, 62, 62, 61, 61, 63, 58, 58, 58};

// Expected Result

int16_t exp_res[64] =
    {699, 164, -51, -16, 31, -15, -19, 8, 71, 14, -61, -2, 11, -12, 7, 12, -58, -55, 13, 28, -20, -7, 14, -18, 29, 22, 3, 3, -11, 7, 11, -22, -1, -28, -27, 10, 0, -7, 11, 6, 7, 6, 21, 21, -10, -8, 2, -14, 1, -7, -15, -15, -10, 15, 16, -10, 0, -1, 0, 15, 4, -13, -5, 4};

// Fast Discrete Cosine Transform

void fdct(int16_t block[], int lx) {
    int32_t tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp10, tmp11, tmp12, tmp13;
    int32_t z1, z2, z3, z4, z5;
    int32_t constant;

    // Pass 1: process rows.

    for (int i = 0; i < 8; i++) {
        const int base = i * lx;

        tmp0 = block[base + 0] + block[base + 7];
        tmp7 = block[base + 0] - block[base + 7];
        tmp1 = block[base + 1] + block[base + 6];
        tmp6 = block[base + 1] - block[base + 6];
        tmp2 = block[base + 2] + block[base + 5];
        tmp5 = block[base + 2] - block[base + 5];
        tmp3 = block[base + 3] + block[base + 4];
        tmp4 = block[base + 3] - block[base + 4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        block[base + 0] = ((tmp10 + tmp11) << PASS1_BITS);
        block[base + 4] = ((tmp10 - tmp11) << PASS1_BITS);

        constant = 4433;
        z1 = (tmp12 + tmp13) * constant;
        constant = 6270;
        block[base + 2] = (z1 + (tmp13 * constant)) >> (CONST_BITS - PASS1_BITS);
        constant = -15137;
        block[base + 6] = (z1 + (tmp12 * constant)) >> (CONST_BITS - PASS1_BITS);

        // Odd part per figure 8 --- note paper omits factor of sqrt(2).
        // cK represents cos(K*pi/16).
        // i0..i3 in the paper are tmp4..tmp7 here.

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        constant = 9633;
        z5 = ((z3 + z4) * constant); // sqrt(2) * c3

        constant = 2446;
        tmp4 = (tmp4 * constant); // sqrt(2) * (-c1+c3+c5-c7)
        constant = 16819;
        tmp5 = (tmp5 * constant); // sqrt(2) * ( c1+c3-c5+c7)
        constant = 25172;
        tmp6 = (tmp6 * constant); // sqrt(2) * ( c1+c3+c5-c7)
        constant = 12299;
        tmp7 = (tmp7 * constant); // sqrt(2) * ( c1+c3-c5-c7)
        constant = -7373;
        z1 = (z1 * constant); // sqrt(2) * (c7-c3)
        constant = -20995;
        z2 = (z2 * constant); // sqrt(2) * (-c1-c3)
        constant = -16069;
        z3 = (z3 * constant); // sqrt(2) * (-c3-c5)
        constant = -3196;
        z4 = (z4 * constant); // sqrt(2) * (c5-c3)

        z3 += z5;
        z4 += z5;

        block[base + 7] = (tmp4 + z1 + z3) >> (CONST_BITS - PASS1_BITS);
        block[base + 5] = (tmp5 + z2 + z4) >> (CONST_BITS - PASS1_BITS);
        block[base + 3] = (tmp6 + z2 + z3) >> (CONST_BITS - PASS1_BITS);
        block[base + 1] = (tmp7 + z1 + z4) >> (CONST_BITS - PASS1_BITS);
    }

    // Pass 2: process columns.

    for (int i = 0; i < 8; i++) {
        tmp0 = block[0 * lx + i] + block[7 * lx + i];
        tmp7 = block[0 * lx + i] - block[7 * lx + i];
        tmp1 = block[1 * lx + i] + block[6 * lx + i];
        tmp6 = block[1 * lx + i] - block[6 * lx + i];
        tmp2 = block[2 * lx + i] + block[5 * lx + i];
        tmp5 = block[2 * lx + i] - block[5 * lx + i];
        tmp3 = block[3 * lx + i] + block[4 * lx + i];
        tmp4 = block[3 * lx + i] - block[4 * lx + i];

        // Even part per LL&M figure 1 --- note that published figure is faul
        // rotator "sqrt(2)*c1" should be "sqrt(2)*c6".

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        block[0 * lx + i] = (tmp10 + tmp11) >> (PASS1_BITS + 3);
        block[4 * lx + i] = (tmp10 - tmp11) >> (PASS1_BITS + 3);

        constant = 4433;
        z1 = ((tmp12 + tmp13) * constant);
        constant = 6270;
        block[2 * lx + i] = (z1 + (tmp13 * constant)) >> (CONST_BITS + PASS1_BITS + 3);
        constant = -15137;
        block[6 * lx + i] = (z1 + (tmp12 * constant)) >> (CONST_BITS + PASS1_BITS + 3);

        // Odd part per figure 8 --- note paper omits factor of sqrt(
        // cK represents cos(K*pi/16).
        // i0..i3 in the paper are tmp4..tmp7 here.

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        constant = 9633;
        z5 = ((z3 + z4) * constant); // sqrt(2) * c3

        constant = 2446;
        tmp4 = (tmp4 * constant); // sqrt(2) * (-c1+c3+c5-c7)
        constant = 16819;
        tmp5 = (tmp5 * constant); // sqrt(2) * ( c1+c3-c5+c7)
        constant = 25172;
        tmp6 = (tmp6 * constant); // sqrt(2) * ( c1+c3+c5-c7)
        constant = 12299;
        tmp7 = (tmp7 * constant); // sqrt(2) * ( c1+c3-c5-c7)
        constant = -7373;
        z1 = (z1 * constant); // sqrt(2) * (c7-c3)
        constant = -20995;
        z2 = (z2 * constant); // sqrt(2) * (-c1-c3)
        constant = -16069;
        z3 = (z3 * constant); // sqrt(2) * (-c3-c5)
        constant = -3196;
        z4 = (z4 * constant); // sqrt(2) * (c5-c3)

        z3 += z5;
        z4 += z5;

        block[7 * lx + i] = (tmp4 + z1 + z3) >> (CONST_BITS + PASS1_BITS + 3);
        block[5 * lx + i] = (tmp5 + z2 + z4) >> (CONST_BITS + PASS1_BITS + 3);
        block[3 * lx + i] = (tmp6 + z2 + z3) >> (CONST_BITS + PASS1_BITS + 3);
        block[1 * lx + i] = (tmp7 + z1 + z4) >> (CONST_BITS + PASS1_BITS + 3);
    }
}

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uint64_t initTime = readTime();

    int16_t dummyArray[64];
    volatile int16_t(*dummySink)[64] = &dummyArray;

    const uint32_t amountTests = 500;
    for (int i = 0; i < amountTests; i++) {
        int16_t j = 0;
        while (j < 10) {
            int16_t input[64] =
                {99, 104, 109, 113, 115, 115, 55, 55, 104, 111, 113, 116, 119, 56, 56, 56, 110, 115, 120, 119, 118, 56, 56, 56, 119, 121, 122, 120, 120, 59, 59, 59, 119, 120, 121, 122, 122, 55, 55, 55, 121, 121, 121, 121, 60, 57, 57, 57, 122, 122, 61, 63, 62, 57, 57, 57, 62, 62, 61, 61, 63, 58, 58, 58};

            for (int k = 0; k < 64; k++) {
                input[k] += j;
            }

            fdct(input, 8);

            for (int x = 0; x < 64; x++) {
                (*dummySink)[x] = input[x];
            }

            j += 1;
        }
    }

    const uint64_t finishTime = readTime() - initTime;

    uart0Init();
    uartSendString("\r\nc finite dicrete cosine transform : ");
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(finishTime);
    uartSendString(" microseconds");
}
