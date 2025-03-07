MEMORY
{
    sram : ORIGIN = 0x20000000, LENGTH = 0xFFFF
}

SECTIONS
{
    .text.main : {
        *(.main)
    } > sram

    .text : {
        *(.text*)
    } > sram
}

