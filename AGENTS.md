# Introduction
Gestalt-tests is an AI engineering project that leverages AI to generate system-level tests(C, ASM, C++) for RISC-V.

# Project structure
project root directory is gestalt-tests
```shell
./assistant Used to store different AI agents.
./src Used to store source codes.
./knowledge Used to store reference projects and documents.
./blueprint Used to manage test prompts.
./scripts Used to store shell command scripts.
./docs Used to store test documents.
```

# C/C++ Compiler
```shell
export export RISCV=~/riscv
```

riscv toolchain is here:
```shell
export $RISCV/bin/riscv64-unknown-elf-
```
