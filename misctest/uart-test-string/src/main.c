#include "io.h"
#include "uart.h"

#include <stdint.h>

#define GPIO25_CTRL 0x0cc

void main(void) __attribute__((section(".main")));
void main() {
    uart0Init();
    timerInit();

    write32((volatile uint32_t *)(IO_BANK0_BASE + GPIO25_CTRL), 5);
    write32((volatile uint32_t *)(0xd0000000 + 0x024), (1 << 25));
    write32((volatile uint32_t *)(0xd0000000 + 0x014), (1 << 25));

    uartSendString("hello world");
    uartSendString("let's try a long string here something something something that is a bit long and would give us something something something yes let's have a bit of a longer string yes yes yes longer string");
    /*uartSendInt(5);*/
    /*uartSendInt(-32120);*/
    /*uartSendInt(1239123);*/
    /*uartSendInt(-912382);*/

    uartSendString("\n");

    int i = 2128321;

    if (i % 2128321 == 0) {
        uartSend('Y');
    }

    int j = readTime();
    j += 1;

    if (i % 21231291 != 0) {
        uartSend('Y');
    }

    uartSendString("TESTER");
    uartSendString("TESTER");

    for (int x = 1; x < 100; x++) {
        uartSendString("LOOP THROUGH");
        if (x % 50 == 0) {
            uartSendString("OKAY");
        }
    }

    uartSendString("DONE");
}
