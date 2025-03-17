ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O2 -ffreestanding -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections
LINK_ARGS = -nostdlib --gc-sections
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart
LINKER = $(LIBRARIES)/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c src/main.c -o out/main.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/uart/uart.c -o out/uart.o

link:
	$(ARM)-ld $(LINK_ARGS) -T $(LINKER) out/main.o out/io.o out/uart.o -o out/main.elf
	$(ARM)-objdump -D out/main.elf > out/main.asm

convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2

