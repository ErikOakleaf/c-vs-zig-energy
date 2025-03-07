ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O2 -ffreestanding -mcpu=cortex-m0plus -mthumb
LINK_ARGS = -nostdlib

default:
	$(ARM)-gcc $(GCC_ARGS) -c src/main.c -o out/main.o
	$(ARM)-ld $(LINK_ARGS) -T src/linker.ld out/main.o -o out/main.elf
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	$(ARM)-objdump -D out/main.elf > out/main.asm
	uf2-util --bootrom include/bootrom.bin --progdata out/main.bin --output out/app.uf2

