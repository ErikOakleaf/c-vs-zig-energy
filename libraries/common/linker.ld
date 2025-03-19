ENTRY(main)

MEMORY
{
    sram : ORIGIN = 0x20000000, LENGTH = 0x42000
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

