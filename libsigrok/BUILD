cc_library(
    name = "libsigrok",
    srcs = glob(
        [
            "src/**/*.c",
            "src/**/*.h",
        ],
        exclude = [
            "src/hardware/**",
            "src/scpi/**",
        ],
    ) + [
        "config.h",
        "src/scpi/scpi.c",
        "src/scpi/scpi_serial.c",
        "src/scpi/scpi_tcp.c",
        #"src/scpi/scpi_vxi.c",
        "src/scpi/vxi.h",
        "src/scpi/scpi_usbtmc_libusb.c",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = [
        ".",
        "include",
        "src",
        "src/scpi",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//libserialport",
        "@glib//glib",
        "@libusb",
        "@libzip",
    ],
)

cc_library(
    name = "cxx",
    hdrs = glob(["bindings/cxx/include/libsigrokcxx/**/*.hpp"]),
    strip_include_prefix = "bindings/cxx/include",
    visibility = ["//visibility:public"],
    deps = [
        ":libsigrok",
    ],
)
