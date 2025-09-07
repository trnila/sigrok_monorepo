load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")


exports_files(["ftdi_version_i.h"])

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = [
        "//sigrok-cli",
    ],
)