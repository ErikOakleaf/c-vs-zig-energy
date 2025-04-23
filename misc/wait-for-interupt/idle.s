    .syntax unified
    .cpu cortex-m0
    .thumb

    .global main
    .type   main, %function

main:
    cpsie   i       // Enable interrupts so WFI can wake
1:
    wfi             // Halt until an IRQ or debug event
    b   1b          // Loop back
