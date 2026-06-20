#!/usr/bin/env bash
###############################################################################
# run_clean_build.sh
#
# Builds and runs the hello-engflow target inside a clean Docker container
# that lacks system zlib headers, simulating a remote execution worker.
#
# Expected outcome (before applying the hermetic fix):
#   The build FAILS with:
#       fatal error: zlib.h: No such file or directory
#
# After swapping in MODULE.bazel.hermetic and BUILD.hermetic:
#   The build SUCCEEDS because zlib is fetched and compiled from source.
###############################################################################

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

IMAGE_NAME="clean-runner"
DOCKERFILE="Dockerfile.clean-runner"

# Get host architecture
ARCH=$(uname -m)
if [ "$ARCH" = "arm64" ] || [ "$ARCH" = "aarch64" ]; then
    TARGET_ARCH="arm64"
else
    TARGET_ARCH="amd64"
fi

echo "═══════════════════════════════════════════════════════════════════════"
echo "  🐳  Building clean-runner Docker image (no zlib installed)..."
echo "═══════════════════════════════════════════════════════════════════════"
docker build --build-arg TARGETARCH="$TARGET_ARCH" -t "$IMAGE_NAME" -f "$DOCKERFILE" .

echo ""
echo "═══════════════════════════════════════════════════════════════════════"
echo "  🔨  Running 'bazel build //...' inside the clean container..."
echo "═══════════════════════════════════════════════════════════════════════"
echo ""

# Mount the project directory into the container's /app working directory.
# --network=host allows the container to reach the local remote cache
# if docker-compose is also running.
docker run --rm \
    --network=host \
    -v "$(pwd)":/app \
    -w /app \
    "$IMAGE_NAME" \
    bazel build //...

echo ""
echo "═══════════════════════════════════════════════════════════════════════"
echo "  ✅  Build completed successfully inside the clean container."
echo "═══════════════════════════════════════════════════════════════════════"
