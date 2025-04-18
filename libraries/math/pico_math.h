#ifndef PICOMATH_H
#define PICOMATH_H


#include "io.h"
#include <stdint.h>

#define SIO_BASE 0xd0000000
#define DIV_UDIVIDEND (SIO_BASE + 0x60)
#define DIV_UDIVISOR (SIO_BASE + 0x64)
#define DIV_QUOTIENT (SIO_BASE + 0x70)
#define DIV_REMAINDER (SIO_BASE + 0x74)
#define DIV_CSR (SIO_BASE + 0x78)

typedef struct {
    uint32_t quotient;
    uint32_t remainder;
} div_result_t;

uint32_t u32Div(uint32_t dividend, uint32_t divisor); 
uint32_t u32Mod(uint32_t dividend, uint32_t divisor); 
div_result_t u32DivMod(uint32_t dividend, uint32_t divisor); 

#endif 
