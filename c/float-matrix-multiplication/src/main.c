#include "test_matrices.h"
#include "uart.h"
#include <stdint.h>

void matrixMultiply5x5(int matrix1[5][5], int matrix2[5][5], int result[5][5]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 5; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void testChecksum(int result[5][5], int checksum) {
    int compare = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            compare += result[j][i];
        }
    }

    if (!(compare == checksum)) {
        uart0Init();
        uartSendString("checksum error");
        uartSendString("\r\n");
    }
}

void printResult(int result[5][5]) {
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

    int amountTests = 500;

    int result[5][5];

    for (int i = 0; i < amountTests; i++) {
        for (int j = 0; j < NUM_TEST_MATRICES; j++) {
            matrixMultiply5x5(test_matrices[j].matrix1, test_matrices[j].matrix2, result);
            testChecksum(result, test_matrices[j].result_checksum);
        }
    }

    uart0Init();
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(readTime());
    uartSendString(" microseconds");
}
