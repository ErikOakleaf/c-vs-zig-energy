#include "io.h"
#include "pico_math.h"
#include "uart.h"

#include <stdint.h>

void main(void) __attribute__((section(".main")));
void main() {
    uart0Init();
    timerInit();
	uint32_t t = readTime();
    uartSendU32(t);
}
