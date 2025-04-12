#include "uart.h"
#include <stdint.h>

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

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uint64_t initTime = readTime();


    double dummyArray[5][5];
    volatile double (*dummySink)[5][5] = &dummyArray;

    double result[5][5];

    double input1[5][5];
    double input2[5][5];

    int amountTests = 500;
    for (int i = 0; i < amountTests; i++) {
        double j = 0.5;
        while (j < 10.5) {
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
    uartSendString("\r\nc float matrix multiplication : ");
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(finishTime);
    uartSendString(" microseconds");
}
