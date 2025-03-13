pub fn write32(address: *volatile u32, value: u32) void {
    address.* = value;
}

pub fn read32(address: *volatile u32) u32 {
    return address.*;
}
