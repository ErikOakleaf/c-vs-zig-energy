const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{
        .default_target = .{
            .cpu_arch = .thumb,
            .os_tag = .freestanding,
            .abi = .none,
            .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_m0plus },
        },
    });

    const optimize = std.builtin.OptimizeMode.ReleaseSmall;

    const obj = b.addObject(.{
        .name = "main",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    const io = b.createModule(.{ .root_source_file = .{ .cwd_relative = "../../libraries/io/io.zig" } });

    const math = b.createModule(.{ .root_source_file = .{ .cwd_relative = "../../libraries/math/pico_math.zig" } });
    math.addImport("io", io);

    const uart = b.createModule(.{ .root_source_file = .{ .cwd_relative = "../../libraries/uart/uart.zig" } });
    uart.addImport("io", io);
    uart.addImport("pico_math", math);

    const test_data = b.createModule(.{ .root_source_file = .{ .cwd_relative = "../../test-data/dijkstras/test_data.zig" } });

    obj.root_module.addImport("io", io);
    obj.root_module.addImport("uart", uart);
    obj.root_module.addImport("pico_math", math);
    obj.root_module.addImport("test_data", test_data);

    const install_obj = b.addInstallFile(obj.getEmittedBin(), "main.o");
    b.getInstallStep().dependOn(&install_obj.step);
    const install_asm = b.addInstallFile(obj.getEmittedAsm(), "main.asm");
    b.getInstallStep().dependOn(&install_asm.step);
}
