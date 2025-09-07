cc_binary(
    name = "sigrok-cli",
    srcs = glob([
        "**/*.c",
        "**/*.h",
    ]),
    includes = ["."],
    deps = [
        "//libsigrok",
        "//libsigrokdecode",
        "@glib//glib",
    ],
)
