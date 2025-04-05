ENTRY(main)

MEMORY
{
    sram (rx) : ORIGIN = 0x20000000, LENGTH = 0x42000
}

SECTIONS
{
    .text.main : {
        KEEP(*(.main))
    } > sram

    .text : {
        KEEP(*(.text*))
    } > sram
}

