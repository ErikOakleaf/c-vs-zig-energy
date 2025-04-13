ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O3 -ffreestanding -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections
CLANG_ARGS = --target=arm-none-eabi -mcpu=cortex-m0plus -march=armv6-m -mthumb -Wall -ffunction-sections -fdata-sections -O3 -ffast-math
LINK_ARGS = -nostdlib -Wl,--gc-sections
LINK_ARGS_CLANG = --target=arm-none-eabi -mcpu=cortex-m0plus -march=armv6-m -nostdlib
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart
LINKER = $(LIBRARIES)/common/linker.ld
LINKER_CLANG = $(LIBRARIES)/common/linker.ls

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c src/main.c -o out/main.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io.o
	clang $(CLANG_ARGS) $(INCLUDES) -c src/main.c -o out/main_clang.o 
	clang $(CLANG_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io_clang.o 

link:
	$(ARM)-gcc $(LINK_ARGS) -T $(LINKER) out/main.o out/io.o -o out/main.elf
	$(ARM)-objdump -D out/main.elf > out/main.asm
	clang $(LINK_ARGS_CLANG)  out/main_clang.o out/io_clang.o -nostdlib -T$(LINKER) -o out/main_clang.elf
	$(ARM)-objdump -D out/main_clang.elf > out/main_clang.asm


convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2
	$(ARM)-objcopy -O binary out/main_clang.elf out/main_clang.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main_clang.bin --output out/app_clang.uf2

