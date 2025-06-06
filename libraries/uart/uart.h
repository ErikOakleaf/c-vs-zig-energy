#ifndef UART_H  
#define UART_H 

#include "io.h"
#include <stdint.h>

typedef unsigned long long uint_64;

#define RESETS_BASE 0x4000c000
#define CLOCK_BASE 0x40008000
#define CLK_PERI_CTRL 0x48 // offset for stetting up periphiral clock
#define IO_BANK0_BASE 0x40014000
#define GPIO0_CTRL 0x4
#define GPIO1_CTRL 0xc
#define UART0_BASE 0x40034000

void resetUart0(); 
void resetIOBank0();
void enablePeriClock();
void functionSelectUart();
void setBaudRate();
void enableFIFOs();
void enableTransmitReceive();
void uart0Init();
void uartSend(char character);
void uartSendString(char *str);
void uartSendU32(uint32_t num);
char uartReceive();

#endif 
