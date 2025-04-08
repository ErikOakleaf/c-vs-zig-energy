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

    obj.root_module.addImport("io", io);
    obj.root_module.unwind_tables = .none;

    const install_obj = b.addInstallFile(obj.getEmittedBin(), "main.o");
    b.getInstallStep().dependOn(&install_obj.step);
}
