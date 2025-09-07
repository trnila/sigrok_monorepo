cc_binary(
    name = "pulseview",
    srcs = glob(
        [
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.ui",
        ],
        exclude = [
            "android/**",
        ],
    ),
    copts = [
        #"-I/usr/include/glibmm-2.4/",
    ],
    includes = [
        ".",
        "pv/data",
    ],
    deps = [
        "//libsigrok:cxx",
        "//libsigrokdecode",
        "@glibmm",
        "@libsigcplusplus",
        "@glib//glib",
        "@glib//gobject",
        "@rules_qt//:qt",
    ],
)
