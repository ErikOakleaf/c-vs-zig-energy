ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O2 -ffreestanding -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections
LINK_ARGS = -nostdlib -Wl,--gc-sections 
LIBGCC = C:\ProgramData\chocolatey\lib\gcc-arm-embedded\tools\gcc-arm-none-eabi-10.3-2021.10\lib\gcc\arm-none-eabi\10.3.1\thumb\v6-m\nofp\libgcc.a
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart -I$(LIBRARIES)/math
LINKER = $(LIBRARIES)/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c src/main.c -o out/main.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/math/pico_math.c -o out/math.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/uart/uart.c -o out/uart.o

link:
	$(ARM)-gcc $(LINK_ARGS) -T $(LINKER) out/main.o out/io.o out/uart.o out/math.o -o out/main.elf 
	$(ARM)-objdump -D out/main.elf > out/main.asm

convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2

