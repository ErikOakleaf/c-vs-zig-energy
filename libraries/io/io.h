#ifndef IO_H
#define IO_H

#define RESETS_BASE 0x4000c000
#define TIMER_BASE 0x40054000
#define TIMER_TIME_LOW (TIMER_BASE + 0x28)
#define TIMER_TIME_HIGH (TIMER_BASE + 0x24)

typedef unsigned int uint_32;
typedef unsigned long long uint_64;

void write32(volatile uint_32 *address, uint_32 value);
uint_32 read32(volatile uint_32 *address);
void timerInit(); 
uint_64 readTime(); 

#endif
