entrypoint:
    ldr r0,=0x20040000
    mov sp,r0 // puth the stack pointer at the end of sram
    bl boot
    b .
