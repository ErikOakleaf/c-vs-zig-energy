#ifndef IO_H  
#define IO_H 

typedef unsigned int uint_32;

void write32(volatile uint_32 *address, uint_32 value);
uint_32 read32(volatile uint_32 *address); 

#endif 
