ARM = arm-none-eabi
AS_ARGS = --warn --fatal-warnings -mcpu=cortex-m0
GCC_ARGS = -Wall -Werror -O2 -ffreestanding -mcpu=cortex-m0 -mthumb
LINK_ARGS = -nostdlib

defualt:
	$(ARM)-as $(AS_ARGS) ssbootloader.s -o out/start.o
	$(ARM)-gcc $(GCC_ARGS) -c ssbootloader.c -o out/bootrom.o
	$(ARM)-ld $(LINK_ARGS) out/start.o out/bootrom.o -o out/bootrom.elf
	@$(ARM)-objcopy -O binary out/bootrom.elf out/bootrom.bin
	@$(ARM)-objdump -D out/bootrom.elf > out/bootrom.asm
