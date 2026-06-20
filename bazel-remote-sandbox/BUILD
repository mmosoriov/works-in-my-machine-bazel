###############################################################################
#
# STAGE: "Reproduce Failure"
#
###############################################################################

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "hello-engflow",
    srcs = ["src/main.cpp"],
    # Locate zlib on host and link to it
    linkopts = ["-lz"],
)
