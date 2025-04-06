#include "test_matrices.h"
#include "uart.h"
#include <stdint.h>
#define EPSILON 1e-6

void matrixMultiply5x5(double matrix1[5][5], double matrix2[5][5], double result[5][5]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 5; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void generateChecksum(double result[5][5], double *checksum) {
    *checksum = 0;
    // Make sure this summation order matches how the reference checksums were calculated
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            *checksum += result[i][j];
        }
    }
}

void printResult(double result[5][5]) {
    uart0Init();
    int compare = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            compare += result[j][i];
        }
    }

    uartSendU32((uint32_t)compare);
    uartSendString("\r\n");

    /*for (int i = 0; i < 5; i++) {*/
    /*    uartSendString("\r\n");*/
    /*    for (int j = 0; j < 5; j++) {*/
    /*        uartSendU32((uint32_t)result[j][i]);*/
    /*        uartSend(' ');*/
    /*    }*/
    /*}*/
}

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uint64_t initTime = readTime();

    int amountTests = 10;
    double checksum;

    double result[5][5];

    for (int i = 0; i < amountTests; i++) {
        for (int j = 0; j < NUM_TEST_MATRICES; j++) {
            matrixMultiply5x5(test_matrices[j].matrix1, test_matrices[j].matrix2, result);
            generateChecksum(result, &checksum);
        }
    }

    uart0Init();
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(readTime() - initTime);
    uartSendString(" microseconds");
}
