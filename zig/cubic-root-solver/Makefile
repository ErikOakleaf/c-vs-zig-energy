ARM = arm-none-eabi
GCC_ARGS = -Wall -Werror -O2 -ffreestanding -mcpu=cortex-m0plus -mthumb
BOOTROM = ../../ssbootloader/out/bootrom.bin
LIBRARIES = ../../libraries
INCLUDES = -I$(LIBRARIES) -I$(LIBRARIES)/io -I$(LIBRARIES)/uart
LINKER = $(LIBRARIES)/common/linker.ld

default: setup compile link convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	zig build --verbose

convert:
	$(ARM)-objcopy -O binary zig-out/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2

