///////////////////////////////////////////////////////////////////////////////
// main.cpp — Compresses a string using zlib library
//
// On a developer's laptop, the header is found and the build succeeds.
// On a clean remote execution worker that lacks zlib headers, the compiler will
// emit:
//     fatal error: zlib.h: No such file or directory
///////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

// System dependency —
#include <zlib.h>

int main() {
  std::printf("Hello, EngFlow! Starting build performance check...\n\n");

  // Compress a test string to prove that the zlib linkage works.
  // We use a longer string with repeated patterns so DEFLATE can find
  // redundancy and achieve meaningful compression (short unique strings
  // are larger after compression due to zlib's ~11-byte header overhead).
  const char *original =
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "EngFlow accelerates builds by 10x or more! "
      "Remote caching and remote execution eliminate redundant work. "
      "Remote caching and remote execution eliminate redundant work. "
      "Remote caching and remote execution eliminate redundant work. "
      "Remote caching and remote execution eliminate redundant work. "
      "Hermetic builds ensure reproducibility across all environments.";
  const uLong original_len = static_cast<uLong>(std::strlen(original));

  // compressBound() returns an upper bound on the compressed size.
  uLong compressed_len = compressBound(original_len);
  std::vector<Bytef> compressed(compressed_len);

  // compress() performs raw DEFLATE compression.
  int result =
      compress(compressed.data(), &compressed_len,
               reinterpret_cast<const Bytef *>(original), original_len);

  if (result != Z_OK) {
    std::fprintf(stderr, "Error: zlib compress() failed with code %d\n",
                 result);
    return EXIT_FAILURE;
  }

  std::printf("  Original  string : \"%.60s...\"\n", original);
  std::printf("  Original  size   : %lu bytes\n",
              static_cast<unsigned long>(original_len));
  std::printf("  Compressed size  : %lu bytes\n",
              static_cast<unsigned long>(compressed_len));
  std::printf("  Compression ratio: %.1f%%\n",
              100.0 * (1.0 - static_cast<double>(compressed_len) /
                                 static_cast<double>(original_len)));
  std::printf("\nBuild performance check complete. All systems nominal.\n");

  return EXIT_SUCCESS;
}
