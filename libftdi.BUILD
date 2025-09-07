cc_library(
    name = "libftdi",
    srcs = glob(["**/*.c"]),
    hdrs = glob(["**/*.h"]) + ["@@//:ftdi_version_i.h"],
    includes = ["src"],
    visibility = ["//visibility:public"],
    deps = [
        "@libusb",
        "@confuse",
    ]
)