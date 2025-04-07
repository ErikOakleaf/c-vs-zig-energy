const RESETS_BASE: u32 = 0x4000c000;
const TIMER_BASE: u32 = 0x40054000;
const TIMER_TIME_LOW: u32 = (TIMER_BASE + 0x28);
const TIMER_TIME_HIGH: u32 = (TIMER_BASE + 0x24);
const RANDOM_BIT: u32 = (0x40060000 + 0x1C);

pub fn write32(address: *volatile u32, value: u32) void {
    address.* = value;
}

pub fn read32(address: *volatile u32) u32 {
    return address.*;
}

pub fn timerInit() void {
    write32(@ptrFromInt(RESETS_BASE + 0x3000), 1 << 21);
    while ((read32(@ptrFromInt(RESETS_BASE + 0x8)) & (1 << 21)) == 0) {}
}

pub fn readTime() u64 {
    var high: u32 = 0;
    var low: u32 = 0;

    while (true) {
        high = read32(@ptrFromInt(TIMER_TIME_HIGH));
        low = read32(@ptrFromInt(TIMER_TIME_LOW));
        if (high == read32(@ptrFromInt(TIMER_TIME_HIGH))) {
            break;
        }
    }

    const high64: u64 = @intCast(high);

    return (high64 << 32) | low;
}

pub fn getRandom() usize {
    return @intCast((read32(@ptrFromInt(RANDOM_BIT))) & 0b1);
}
