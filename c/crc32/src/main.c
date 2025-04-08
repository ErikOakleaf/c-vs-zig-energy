#include "io.h"
#include "uart.h"
#include <stdint.h>

#define POLYNOM 0xEDB88320

void crc32(const char input[], int size, uint32_t *output) {
    uint32_t crc = 0xFFFFFFFF;

    for(int i = 0; i < size; i++) {
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
    /*uint64_t initTime = readTime();*/
    /*uint32_t amountTests = 500;*/

    uint32_t output;

    crc32("h", 1, &output);



    uart0Init();
    uartSendU32(output);
    /*uartSendU32(amountTests);*/
    /*uartSendString(" tests done, took: ");*/
    /*uartSendU32(readTime() - initTime);*/
    /*uartSendString(" microseconds");*/
}
