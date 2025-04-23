ARM = arm-none-eabi
GCC_ARGS = -mcpu=cortex-m0plus -mthumb -march=armv6-m 
LINK_ARGS = -mcpu=cortex-m0plus -march=armv6-m -nostartfiles -nostdlib -ffreestanding
BOOTROM = ../../ssbootloader/out/bootrom.bin
LINKER = ../../libraries/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-as $(GCC_ARGS) idle.s -o out/main.o

link:
	$(ARM)-gcc $(LINK_ARGS) -T $(LINKER) out/main.o -o out/main.elf 
	$(ARM)-objdump -D out/main.elf > out/main.asm

convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2
