#include "uart.h"

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

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();

    uart0Init();
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(readTime());
    uartSendString(" microseconds");
}
