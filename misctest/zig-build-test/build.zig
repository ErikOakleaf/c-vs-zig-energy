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

    const exe = b.addExecutable(.{
        .name = "main.elf",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    const io = b.createModule(.{ .root_source_file = .{ .cwd_relative = "../../libraries/io/io.zig" } });

    exe.root_module.addImport("io", io);
    exe.root_module.unwind_tables = .none;
    exe.linker_script = .{ .cwd_relative = "../../libraries/common/linker.ld" };

    const dissasmble_cmd = b.addSystemCommand(&[_][]const u8{
        "arm-none-eabi-objdump",
        "-D",
    });
    dissasmble_cmd.addFileArg(exe.getEmittedBin());
    const dissasmble_file = dissasmble_cmd.captureStdOut();
    const install_asm = b.addInstallFile(dissasmble_file, "main.asm");

    b.installArtifact(exe);
    b.getInstallStep().dependOn(&install_asm.step);
}
