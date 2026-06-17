#!/bin/bash
# AI[DeepSeek V4 Pro, high] GENERATED BEGIN
# Run script: simulate gestalt-tests hello test with Spike (riscv-isa-sim)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
SPIKE="${PROJECT_DIR}/knowledge/riscv-isa-sim/build/spike"
TEST_DIR="${PROJECT_DIR}/src/hello"
BINARY="${TEST_DIR}/build/hello-riscv64-cb-v1.bin"
ELF="${TEST_DIR}/build/hello-riscv64-cb-v1.elf"

echo "============================================"
echo "  Running hello test on Spike simulator"
echo "============================================"

# Check spike
if [ ! -x "${SPIKE}" ]; then
    echo "ERROR: Spike not found at ${SPIKE}"
    echo "Please build riscv-isa-sim first:"
    echo "  cd ${PROJECT_DIR}/knowledge/riscv-isa-sim && mkdir -p build && cd build && ../configure && make -j\$(nproc)"
    exit 1
fi

# Check binary
if [ ! -f "${BINARY}" ]; then
    echo "ERROR: Binary not found at ${BINARY}"
    echo "Please run build_hello.sh first."
    exit 1
fi

# Run with Spike
# Spike for XiangShan-like config: RV64IMAFDC (from riscv64-cb-v1.mk MARCH)
# --isa=rv64gc covers rv64imafdc_zicsr_zifencei
echo "Spike command: ${SPIKE} --isa=rv64gc ${ELF}"
echo ""

${SPIKE} --isa=rv64gc ${ELF}

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
