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

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uint64_t initTime = readTime();

    int dummyArray[5][5];
    volatile int (*dummySink)[5][5] = &dummyArray;

    int result[5][5];

    int input1[5][5];
    int input2[5][5];

    int amountTests = 25;
    for (int i = 0; i < amountTests; i++) {
        int j = 0;
        while (j < 10) {
            for (int k = 0; k < 5; k++) {
                input1[k][0] = j;
                input1[k][1] = j;
                input1[k][2] = j;
                input1[k][3] = j;
                input1[k][4] = j;

                input2[k][0] = j;
                input2[k][1] = j;
                input2[k][2] = j;
                input2[k][3] = j;
                input2[k][4] = j;
            }

            matrixMultiply5x5(input1, input2, result);

            for (int x = 0; x < 5; x++) {
                for (int y = 0; y < 5; y++) {
                    (*dummySink)[x][y] = result[x][y];
                }
            }

            j += 1;
        }
    }
    uint64_t finishTime = readTime() - initTime;

    uart0Init();
    uartSendString("\r\nc int matrix multiplication : ");
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(finishTime);
    uartSendString(" microseconds");
}
