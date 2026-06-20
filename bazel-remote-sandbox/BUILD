

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "hello-engflow",
    srcs = ["src/main.cpp"],
    # Link against the hermetically-fetched zlib from the BCR.
    # No system library dependency — builds anywhere Bazel runs.
    deps = ["@zlib"],
)
