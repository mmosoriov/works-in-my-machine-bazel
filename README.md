# 🏗️ Bazel "It works on my machine!" Simulation


This project is a self-contained sandbox that **reproduces and then fixes** a classic Bazel build infrastructure issue: a **non-hermetic system library dependency** that compiles on a developer's laptop but fails on a clean remote execution worker.

---

## 🐛 The Issue

The `hello-engflow` C++ binary includes `<zlib.h>` and links against `-lz`:

```cpp
#include <zlib.h>  // ← System header — non-hermetic!
```

On the developer's laptop, `zlib` is typically available. But on a **clean remote worker** , these headers don't exist. The build fails:

```
src/main.cpp:12:10: fatal error: zlib.h: No such file or directory
   12 | #include <zlib.h>
      |          ^~~~~~~~
```

---

## ✅ The Fix

Migrate the build to use a **hermetic** zlib dependency managed entirely by Bazel:

1. **`MODULE.bazel`** — Add `bazel_dep(name = "zlib", version = "1.3.1.bcr.3")` to pull zlib from the [Bazel Central Registry](https://registry.bazel.build/).
2. **`BUILD`** — Replace `linkopts = ["-lz"]` with `deps = ["@zlib"]` to link against the hermetically-built zlib.

Pre-built hermetic versions are provided:
- [`MODULE.bazel.hermetic`](MODULE.bazel.hermetic) — Drop-in replacement for `MODULE.bazel`
- [`BUILD.hermetic`](BUILD.hermetic) — Drop-in replacement for `BUILD`

```bash
# Apply the fix
cp MODULE.bazel.hermetic MODULE.bazel
cp BUILD.hermetic BUILD

# Re-run the clean build — it now succeeds!
./run_clean_build.sh
```

---

## 🚀 Quick Start

### Prerequisites

- [Docker](https://www.docker.com/) & [Docker Compose](https://docs.docker.com/compose/)
- [Bazelisk](https://github.com/bazelbuild/bazelisk) (or Bazel 7+)
- macOS or Linux host with `zlib` installed (for the "works locally" demo)

### 1. Reproduce the Failure

```bash
cd bazel-remote-sandbox

# Step 1: Build locally — should SUCCEED (you have zlib installed)
bazel build //...
bazel run //:hello-engflow

# Step 2: Build in the clean container — should FAIL
./run_clean_build.sh
# Expected error: fatal error: zlib.h: No such file or directory
```

### 2. (Optional) Start the Remote Cache

```bash
# Start the local remote cache
docker compose up -d

# Build with remote caching enabled
bazel build --config=remote_cache //...
```

### 3. Apply the Hermetic Fix

```bash
# Swap in the hermetic configuration
cp MODULE.bazel.hermetic MODULE.bazel
cp BUILD.hermetic BUILD

# Re-run in the clean container — should now SUCCEED
./run_clean_build.sh
```

### 4. Inspect the Build Profile

After any build, open the generated profile in [Perfetto UI](https://ui.perfetto.dev):

```bash
# Load profile.json.gz into Perfetto for a Gantt chart of build actions
open https://ui.perfetto.dev
# Then drag-and-drop profile.json.gz into the browser
```

---

## 📁 File Structure

```
bazel-remote-sandbox/
├── MODULE.bazel              # Bzlmod config (non-hermetic — fails on clean worker)
├── MODULE.bazel.hermetic     # Bzlmod config (hermetic — works everywhere)
├── BUILD                     # Build target (non-hermetic)
├── BUILD.hermetic            # Build target (hermetic)
├── .bazelrc                  # Bazel settings & remote cache config
├── src/
│   └── main.cpp              # C++ binary with zlib dependency
├── docker-compose.yml        # Local bazel-remote HTTP cache
├── Dockerfile.clean-runner   # Clean Ubuntu worker (no zlib)
├── run_clean_build.sh        # Script to run build in clean container
└── README.md                 # This file
```

---

## 🔑 Key Takeaways

1. **Hermetic builds are essential** for remote execution and caching. If your build depends on system-installed libraries, it will fail on workers that don't have them.

2. **Bazel's Bzlmod** makes it easy to declare third-party dependencies that are fetched, built, and linked hermetically — no system packages required.

3. **The Bazel Central Registry (BCR)** hosts pre-configured modules for common libraries like zlib, making migration straightforward.

4. **Remote caching** only works correctly when builds are hermetic. Non-hermetic builds can produce cache poisoning — artifacts compiled against one system's headers cached and served to a different system.

---

## 📚 Further Reading

- [Bazel Remote Execution Overview](https://bazel.build/remote/rbe)
- [Bzlmod Migration Guide](https://bazel.build/external/migration)
- [Bazel Central Registry](https://registry.bazel.build/)
- [EngFlow Remote Execution](https://www.engflow.com/)
- [bazel-remote Cache](https://github.com/buchgr/bazel-remote)

---

*Built as a teaching sandbox for build infrastructure engineers migrating to hermetic, remotely-cacheable Bazel builds.*
