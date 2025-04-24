#include "io.h"
#include "uart.h"
#include <stdint.h>

#define POLYNOM 0xEDB88320

void crc32(const uint8_t input[], int size, uint32_t *output) {
    uint32_t crc = 0xFFFFFFFF;

    for (int i = 0; i < size; i++) {
        crc ^= input[i];

        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ POLYNOM;
            } else {
                crc = crc >> 1;
            }
        }
    }
    *output = crc ^ 0xFFFFFFFF;
}

void main(void) __attribute__((section(".main")));
void main() {
    timerInit();
    uint64_t initTime = readTime();

    uint32_t dummyValue;
    volatile uint32_t *dummySink = &dummyValue;

    uint32_t output;

    uint8_t input[10];

    uint32_t amountTests = 25;
    for (int i = 0; i < amountTests; i++) {
        for (int j = 100; j < 110; j++) {
            input[0] = j;
            input[1] = j;
            input[2] = j;
            input[3] = j;
            input[4] = j;
            input[5] = j;
            input[6] = j;
            input[7] = j;
            input[8] = j;
            input[9] = j;

            crc32(input, 10, &output);
            
            *dummySink = output;
        }
    }


    uint64_t finishTime = readTime() - initTime;

    uart0Init();
    uartSendString("\r\nc crc32 : ");
    uartSendU32(amountTests);
    uartSendString(" tests done, took: ");
    uartSendU32(finishTime);
    uartSendString(" microseconds");
}
