ARM = arm-none-eabi
BOOTROM = ../../ssbootloader/out/bootrom.bin

default: setup compile convert

setup:
	-mkdir out 2>NUL || mkdir -p out 2>/dev/null || echo "out directory already exists"

compile:
	zig build --verbose

convert:
	$(ARM)-objcopy -O binary zig-out/bin/main.elf out/main.bin
	uf2-util --bootrom $(BOOTROM) --progdata out/main.bin --output out/app.uf2

