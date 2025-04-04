ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O3 -ffreestanding -mcpu=cortex-m0plus -mthumb -march=armv6-m -ffunction-sections -fdata-sections
LINK_ARGS = -nostdlib --gc-sections
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
TEST_DATA = ../../test-data
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart -I$(LIBRARIES)/math -I$(TEST_DATA)/int-matrix-multiplication
LINKER = $(LIBRARIES)/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c src/main.c -o out/main.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/uart/uart.c -o out/uart.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/math/pico_math.c -o out/math.o

link:
	$(ARM)-ld $(LINK_ARGS) -T $(LINKER) out/main.o out/io.o out/uart.o out/math.o -o out/main.elf
	$(ARM)-objdump -D out/main.elf > out/main.asm

convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2

