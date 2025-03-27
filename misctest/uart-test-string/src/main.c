#include "io.h"
#include "uart.h"
#include "pico_math.h"

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

    uint32_t i = u32Division(10, 2);
    if (i == 5) {
        uartSendString("TestComplete");
    }

    uint32_t j = u32Mod(10, 3);
    if (j == 1) {
        uartSendString("TestComplete");
    }


    uartSendString("DONE");
}
