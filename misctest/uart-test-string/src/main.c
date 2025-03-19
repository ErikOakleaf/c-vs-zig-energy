#include "io.h"
#include "uart.h"
#define GPIO25_CTRL 0x0cc

void main(void) __attribute__((section(".main")));
void main() {
    uart0Init();

    write32((volatile uint_32 *)(IO_BANK0_BASE + GPIO25_CTRL), 5);
    write32((volatile uint_32 *)(0xd0000000 + 0x024), (1 << 25));
    write32((volatile uint_32 *)(0xd0000000 + 0x014), (1 << 25));

    uartSendString("hello world");
    uartSendString("let's try a long string here something something something that is a bit long and would give us something something something yes let's have a bit of a longer string yes yes yes longer string");
    uartSendInt(5);
    uartSendInt(-32120);
    uartSendInt(1239123);
    uartSendInt(-912382);

}
