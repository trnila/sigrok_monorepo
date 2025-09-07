cc_library(
    name = "libsigrokdecode",
    srcs = glob(
        [
            "**/*.c",
            "**/*.h",
        ],
        exclude = [
            "tests/**",
            "irmp/irmp.c",
        ],
    ),
    hdrs = [
        "irmp/irmp.c",
        "libsigrokdecode.h",
    ],
    includes = [
        ".",
        "irmp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@glib//glib",
        "@rules_python//python/cc:current_py_cc_headers",
        "@rules_python//python/cc:current_py_cc_libs"
    ],
)
