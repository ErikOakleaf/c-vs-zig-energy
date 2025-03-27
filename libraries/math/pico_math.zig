const io = @import("io");

const SIO_BASE: u32 = 0xd0000000;
const DIV_UDIVIDEND: u32 = (SIO_BASE + 0x60);
const DIV_UDIVISOR: u32 = (SIO_BASE + 0x64);
const DIV_QUOTIENT: u32 = (SIO_BASE + 0x70);
const DIV_REMAINDER: u32 = (SIO_BASE + 0x74);
const DIV_CSR: u32 = (SIO_BASE + 0x78);

pub const DivResult = struct {
    quotient: u32,
    remainder: u32,
};

pub fn u32Div(dividend: u32, divisor: u32) u32 {
    io.write32(@ptrFromInt(DIV_UDIVIDEND), dividend);
    io.write32(@ptrFromInt(DIV_UDIVISOR), divisor);

    while ((io.read32(@ptrFromInt(DIV_CSR)) & 1) == 0) {}

    return io.read32(@ptrFromInt(DIV_QUOTIENT));
}

pub fn u32Mod(dividend: u32, divisor: u32) u32 {
    io.write32(@ptrFromInt(DIV_UDIVIDEND), dividend);
    io.write32(@ptrFromInt(DIV_UDIVISOR), divisor);

    while ((io.read32(@ptrFromInt(DIV_CSR)) & 1) == 0) {}

    return io.read32(@ptrFromInt(DIV_REMAINDER));
}

pub fn u32DivMod(dividend: u32, divisor: u32) DivResult {
    io.write32(@ptrFromInt(DIV_UDIVIDEND), dividend);
    io.write32(@ptrFromInt(DIV_UDIVISOR), divisor);

    while ((io.read32(@ptrFromInt(DIV_CSR)) & 1) == 0) {}

    const result: DivResult = DivResult{ .quotient = io.read32(@ptrFromInt(DIV_QUOTIENT)), .remainder = io.read32(@ptrFromInt(DIV_REMAINDER)) };

    return result;
}
