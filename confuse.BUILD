load("@rules_foreign_cc//foreign_cc:configure.bzl", "configure_make")

configure_make(
    name = "confusec",
    configure_in_place = True,
    lib_source = ".",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "confuse",
    srcs = glob(["**/*.c"], exclude=["windows/**", "tests/**"]),
    hdrs = glob(["**/*.h"]),
    includes = ["src"],
    visibility = ["//visibility:public"],
    defines = ['HAVE_FUNOPEN']
)