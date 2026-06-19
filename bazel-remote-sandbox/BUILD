###############################################################################
# BUILD — Top-level build targets for bazel_remote_sandbox
#
# STAGE: "Reproduce Failure"
#
# This BUILD file compiles the hello-engflow binary using only system headers
# and system libraries. It passes -lz to the linker, expecting zlib to be
# available on the host. On a clean remote worker, zlib is absent, causing
# a build failure.
#
# To fix, swap this file with BUILD.hermetic, which links against the
# hermetically-fetched @zlib target instead of the system library.
###############################################################################

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "hello-engflow",
    srcs = ["src/main.cpp"],
    # Link against system zlib (-lz).
    # This is the non-hermetic dependency that will fail on clean workers.
    linkopts = ["-lz"],
)
