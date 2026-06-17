#!/bin/bash
# AI[DeepSeek V4 Pro, high] GENERATED BEGIN
# Run script: simulate gestalt-tests dual-core-hello on Spike with 2 harts

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
SPIKE="${PROJECT_DIR}/knowledge/riscv-isa-sim/build/spike"
TEST_DIR="${PROJECT_DIR}/src/dual-core-hello"
ELF="${TEST_DIR}/build/dual-core-hello-riscv64-cb-v1.elf"

echo "============================================"
echo "  Running dual-core-hello on Spike (2 harts)"
echo "============================================"

# Check spike
if [ ! -x "${SPIKE}" ]; then
    echo "ERROR: Spike not found at ${SPIKE}"
    exit 1
fi

# Check binary
if [ ! -f "${ELF}" ]; then
    echo "ERROR: ELF not found at ${ELF}"
    echo "Please run build_dual-core-hello.sh first."
    exit 1
fi

# Run with Spike: -p2 for 2 processors
echo "Spike command: ${SPIKE} --isa=rv64gc -p2 ${ELF}"
echo ""

${SPIKE} --isa=rv64gc -p2 ${ELF}

EXIT_CODE=$?
echo ""
echo "============================================"
if [ ${EXIT_CODE} -eq 0 ]; then
    echo "  Simulation PASSED (exit code 0)"
else
    echo "  Simulation FAILED (exit code ${EXIT_CODE})"
fi
echo "============================================"
exit ${EXIT_CODE}
# AI GENERATED END
