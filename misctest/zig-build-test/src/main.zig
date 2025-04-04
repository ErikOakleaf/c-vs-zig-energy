const io = @import("io");
const GPIO25_CTRL: u32 = 0x0cc;
const RESETS_BASE: u32 = 0x4000c000;
const IO_BANK0_BASE: u32 = 0x40014000;

fn delay(cycles: u32) void {
    var i: u32 = 0;
    const volatile_ptr: *volatile u32 = @ptrFromInt(0x40000000); // Dummy address for volatile operation
    while (i < cycles) {
        i += 1;
        volatile_ptr.* = i; // Perform a dummy write to a volatile address
    }
}

export fn main() linksection(".main") void {
    io.write32(@ptrFromInt(RESETS_BASE + 0x3000), 1 << 5);
    while (((io.read32(@ptrFromInt(RESETS_BASE + 0x8))) & (1 << 5)) ==
        0)
    {} // wait until the reset done register is written

    io.write32(@ptrFromInt(IO_BANK0_BASE + GPIO25_CTRL), 5);
    io.write32(@ptrFromInt(0xd0000000 + 0x024), (1 << 25));

    while (true) {
        io.write32(@ptrFromInt(0xd0000000 + 0x14), (1 << 25));

        delay(50000);

        io.write32(@ptrFromInt(0xd0000000 + 0x18), (1 << 25));

        delay(50000);
    }
}
