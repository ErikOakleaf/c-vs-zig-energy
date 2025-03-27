#include "pico_math.h"

uint32_t u32Div(uint32_t dividend, uint32_t divisor) {
    write32((volatile uint32_t *)DIV_UDIVIDEND, dividend);
    write32((volatile uint32_t *)DIV_UDIVISOR, divisor);

    while ((read32((volatile uint32_t *)DIV_CSR) & 1) == 0) {
    }

    return read32((volatile uint32_t *)DIV_QUOTIENT);
}

uint32_t u32Mod(uint32_t dividend, uint32_t divisor) {
    write32((volatile uint32_t *)DIV_UDIVIDEND, dividend);
    write32((volatile uint32_t *)DIV_UDIVISOR, divisor);

    while ((read32((volatile uint32_t *)DIV_CSR) & 1) == 0) {
    }

    return read32((volatile uint32_t *)DIV_REMAINDER);
}

div_result_t u32DivMod(uint32_t dividend, uint32_t divisor) {
    write32((volatile uint32_t *)DIV_UDIVIDEND, dividend);
    write32((volatile uint32_t *)DIV_UDIVISOR, divisor);

    while ((read32((volatile uint32_t *)DIV_CSR) & 1) == 0) {
    }

    div_result_t result;
    result.quotient = read32((volatile uint32_t *)DIV_REMAINDER);
    result.remainder = read32((volatile uint32_t *)DIV_REMAINDER);
    return result;
}
