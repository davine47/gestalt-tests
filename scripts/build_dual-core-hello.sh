#!/bin/bash
# AI[DeepSeek V4 Pro, high] GENERATED BEGIN
# Build script for gestalt-tests dual-core-hello test
# Uses nexus-am environment, target: riscv64-cb-v1

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
AM_HOME="${PROJECT_DIR}/knowledge/nexus-am"
TEST_DIR="${PROJECT_DIR}/src/dual-core-hello"

export AM_HOME

# Set RISC-V toolchain path (from AGENTS.md: RISCV=~/riscv)
export RISCV="${RISCV:-$HOME/riscv}"
export PATH="${RISCV}/bin:${PATH}"
export CROSS_COMPILE=riscv64-unknown-elf-

echo "============================================"
echo "  Building dual-core-hello for riscv64-cb-v1"
echo "  AM_HOME = ${AM_HOME}"
echo "============================================"

# Check toolchain
if ! command -v ${CROSS_COMPILE}gcc &> /dev/null; then
    echo "ERROR: RISC-V toolchain not found: ${CROSS_COMPILE}gcc"
    echo "Please set RISC-V toolchain path in your environment."
    exit 1
fi

# Build
cd "${TEST_DIR}"
make -j$(nproc 2>/dev/null || echo 4) ARCH=riscv64-cb-v1

echo ""
echo "Build complete."
echo "Binary: ${TEST_DIR}/build/dual-core-hello-riscv64-cb-v1.bin"
echo "ELF:    ${TEST_DIR}/build/dual-core-hello-riscv64-cb-v1.elf"
# AI GENERATED END
