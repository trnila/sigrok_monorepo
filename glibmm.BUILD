load("@rules_foreign_cc//foreign_cc:meson.bzl", "meson")

filegroup(
    name="data",
    srcs=glob(["**"])
)

meson(
    name = "glibmm",
    lib_source = ":data",
    visibility = ["//visibility:public"],
    deps = [
        "@libsigcplusplus"
    ],
    tags = [
        "requires-network",
    ]
)

cc_library(
    name = "xglibmm",
    srcs = glob([
        "**/*.hpp",
        "**/*.h",
    ]),
    includes = ["glib"],
    strip_include_prefix = "glib",
    visibility = ["//visibility:public"],
)
