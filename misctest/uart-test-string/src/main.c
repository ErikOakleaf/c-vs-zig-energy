#include "io.h"
#include "pico_math.h"
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

    uint32_t i = u32Div(10, 2);
    if (i == 5) {
        uartSendString("TestComplete");
    }

    uint32_t j = u32Mod(10, 3);
    if (j == 1) {
        uartSendString("TestComplete\n");
    }

    uartSendInt(5);

    uartSendString("DONE");
}
