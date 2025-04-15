ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O3 -ffreestanding -mcpu=cortex-m0plus -mthumb -march=armv6-m -ffunction-sections -fdata-sections -ffast-math
CLANG_ARGS = --target=arm-none-eabi -mcpu=cortex-m0plus -march=armv6-m -mthumb -Wall -ffunction-sections -fdata-sections -O3 -ffast-math
LINK_ARGS = -Wl,--gc-sections -mcpu=cortex-m0plus -march=armv6-m
LINK_ARGS_CLANG = --target=arm-none-eabi -mcpu=cortex-m0plus -march=armv6-m -Wl,--gc-sections
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
TEST_DATA = ../../test-data
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart -I$(LIBRARIES)/math 
LINKER = $(LIBRARIES)/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c src/main.c -o out/main.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/uart/uart.c -o out/uart.o
	$(ARM)-gcc $(GCC_ARGS) $(INCLUDES) -c $(LIBRARIES)/math/pico_math.c -o out/math.o
	clang $(CLANG_ARGS) $(INCLUDES) -c src/main.c -o out/main_clang.o 
	clang $(CLANG_ARGS) $(INCLUDES) -c $(LIBRARIES)/io/io.c -o out/io_clang.o 
	clang $(CLANG_ARGS) $(INCLUDES) -c $(LIBRARIES)/uart/uart.c -o out/uart_clang.o 
	clang $(CLANG_ARGS) $(INCLUDES) -c $(LIBRARIES)/math/pico_math.c -o out/math_clang.o 

link:
	$(ARM)-gcc $(LINK_ARGS) -T $(LINKER) -mcpu=cortex-m0plus -mthumb -march=armv6-m out/main.o out/io.o out/uart.o out/math.o -o out/main.elf -lgcc
	$(ARM)-objdump -D out/main.elf > out/main.asm
	clang $(LINK_ARGS_CLANG)  out/main_clang.o out/io_clang.o out/uart_clang.o out/math_clang.o -T$(LINKER) -o out/main_clang.elf
	$(ARM)-objdump -D out/main_clang.elf > out/main_clang.asm

convert:
	$(ARM)-objcopy -O binary out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2
	$(ARM)-objcopy -O binary out/main_clang.elf out/main_clang.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main_clang.bin --output out/app_clang.uf2
