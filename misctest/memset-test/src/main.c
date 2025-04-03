#include "io.h"
#include "pico_math.h"
#include "uart.h"

#include <stddef.h>
#include <stdint.h>

#define GPIO25_CTRL 0x0cc

extern void *__aeabi_memset(void *dest, int c, size_t len);

void main(void) __attribute__((section(".main")));
void main() {
    uart0Init();
    timerInit();

    write32((volatile uint32_t *)(IO_BANK0_BASE + GPIO25_CTRL), 5);
    write32((volatile uint32_t *)(0xd0000000 + 0x024), (1 << 25));
    write32((volatile uint32_t *)(0xd0000000 + 0x014), (1 << 25));

    uint32_t data[16];

    for (int i = 0; i < 16; i++) {
        data[i] = 1;
    }

    uartSendString("Before memset:\r\n");
    for (int i = 0; i < 16; i++) {
        uartSendU32(data[i]);
        uartSendString("\r\n");
    }

    __aeabi_memset(data, 0, (16 * 4));

    uartSendString("After memset:\r\n");
    for (int i = 0; i < 16; i++) {
        uartSendU32(data[i]);
        uartSendString("\r\n");
    }

    uartSendString("DONE");
}
