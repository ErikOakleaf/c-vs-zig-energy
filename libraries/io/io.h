#ifndef IO_H
#define IO_H

#include <stdint.h>

#define RESETS_BASE 0x4000c000
#define TIMER_BASE 0x40054000
#define TIMER_TIME_LOW (TIMER_BASE + 0x28)
#define TIMER_TIME_HIGH (TIMER_BASE + 0x24)
#define RANDOM_BIT (0x40060000 + 0x1C)

typedef unsigned long long uint_64;

void write32(volatile uint32_t *address, uint32_t value);
uint32_t read32(volatile uint32_t *address);
void timerInit(); 
uint_64 readTime(); 
uint32_t getRandom(); 

#endif
