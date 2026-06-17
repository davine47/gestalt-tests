# Hello Test Documentation

## 测试意图

本测试是 gestalt-tests 项目的首个系统级测试，用于验证 RISCV64 裸机环境下基本计算、内存访问、分支预测和缓存功能的正确性。目标架构为 XiangShan 高性能乱序执行 RISCV 处理器。

测试以 "Hello, XiangShan!" 字符串输出为基础，逐步覆盖 XiangShan 微架构的各个关键模块。

## 测试架构

```
src/hello/
├── hello.c       # 主测试程序（10 个子测试）
└── Makefile      # nexus-am 编译规则

scripts/
├── build_hello.sh  # 编译脚本
└── run_hello.sh    # Spike 仿真脚本

docs/
└── hello.md         # 本文档
```

## 子测试详细说明

### TEST 1: Basic character output
- **目标**: 验证 UART 串口输出功能正常
- **覆盖**: AM `_putc()` 接口、klib `printf()` 函数
- **说明**: 输出可打印 ASCII 字符集，验证字符输出一致性

### TEST 2: Data forwarding / pipeline dependency
- **目标**: 测试后端流水线数据前递（forwarding）正确性
- **覆盖**: 寄存器重命名、物理寄存器文件、Tomasulo 数据前递路径
- **XiangShan 对应模块**: `backend/rename`, `backend/regfile`, `backend/exu`
- **说明**: 构造长依赖链（8 级连续 RAW 依赖），每个结果立即被下一条指令使用，验证前递路径无数据错误

### TEST 3: Independent instruction chains (ILP)
- **目标**: 测试后端乱序发射和并行执行能力
- **覆盖**: 发射队列（issue queue）、多功能单元（ALU）
- **XiangShan 对应模块**: `backend/issue`, `backend/exu`
- **说明**: 4 条独立的依赖链同时执行，验证乱序执行结果的正确性

### TEST 4: Branch prediction stress
- **目标**: 测试分支预测单元正确性
- **覆盖**: BPU（分支预测器）、FTQ（Fetch Target Queue）、流水线冲刷
- **XiangShan 对应模块**: `frontend/bpu`, `frontend/ftq`, `backend/rob`
- **说明**: 包含交替分支、模运算分支、嵌套条件判断的复杂控制流，验证预测错误时的流水线恢复

### TEST 5: ICache fetch / FTQ stress
- **目标**: 测试指令缓存和取指目标队列
- **覆盖**: ICache 行填充、FTQ 取指调度、指令预取
- **XiangShan 对应模块**: `frontend/icache`, `frontend/ftq`, `frontend/ifu`
- **说明**: 通过跨代码区域的函数调用模式，触发 ICache 缺失和行填充，验证取指正确性

### TEST 6: DCache access patterns
- **目标**: 测试数据缓存的多种访问模式
- **覆盖**: L1 DCache 读写、写回策略、缓存一致性
- **XiangShan 对应模块**: `cache/dcache`
- **子测试**:
  - 6a: 顺序访问 — 线性地址空间的写入和回读
  - 6b: 跨步访问 — 步长为 8 的访问模式，验证未修改数据保持原值
  - 6c: 伪随机访问 — 使用 Xorshift 生成器模拟随机地址访问

### TEST 7: Memory access sizes
- **目标**: 测试 Load/Store 单元不同数据宽度
- **覆盖**: LSU 字节/半字/字/双字读写
- **XiangShan 对应模块**: `backend/exu` (LSU)
- **说明**: 分别测试 8/16/32/64 位数据宽度的读写正确性

### TEST 8: Nested loops (ITLB/BTB stress)
- **目标**: 测试指令 TLB 和分支目标缓冲
- **覆盖**: ITLB 命中/缺失、BTB 跳转预测
- **XiangShan 对应模块**: `cache/mmu` (ITLB), `frontend/bpu` (BTB)
- **说明**: 三重嵌套循环，每层不同边界和条件分支，验证循环分支预测稳定性

### TEST 9: Bit manipulation (ALU exercise)
- **目标**: 测试 ALU 单元的位操作指令
- **覆盖**: 与/或/异或、移位、算术移位等逻辑操作
- **XiangShan 对应模块**: `backend/exu` (ALU)
- **说明**: 覆盖 RV64 基本位操作指令，验证 ALU 各子单元计算结果正确

### TEST 10: Recursion / RAS stress
- **目标**: 测试返回地址栈（RAS）
- **覆盖**: RAS 预测、函数调用/返回路径
- **XiangShan 对应模块**: `frontend/bpu` (RAS)
- **说明**: 递归调用深度 10，验证 RAS 压栈/弹栈正确性和递归计算结果

## 编译与运行

### 前置条件
- RISC-V 工具链: `riscv64-unknown-elf-gcc` 等
- nexus-am 抽象机环境: `knowledge/nexus-am/`
- Spike 仿真器: `knowledge/riscv-isa-sim/build/spike`

### 编译
```bash
cd gestalt-tests
bash scripts/build_hello.sh
```

### 仿真运行
```bash
bash scripts/run_hello.sh
```

### 预期输出
```
========================================
  Gestalt-test: XiangShan Hello Suite
========================================

[TEST 1] Basic character output
...
[TEST 10] Recursion / RAS stress
  deep_recursion(10) = 97120
[TEST 10] PASSED

========================================
  All tests PASSED!
========================================
```

## XiangShan 微架构覆盖总结

| 模块 | 测试 | 说明 |
|------|------|------|
| Frontend BPU | TEST 4, 10 | 分支预测、RAS |
| Frontend FTQ | TEST 4, 5 | 取指目标队列 |
| Frontend ICache | TEST 5 | 指令缓存 |
| Frontend IFU | TEST 5, 8 | 取指单元 |
| Frontend ITLB | TEST 8 | 指令 TLB |
| Backend Rename | TEST 2, 3 | 寄存器重命名 |
| Backend Issue | TEST 3 | 发射队列 |
| Backend EXU/ALU | TEST 2, 9 | 执行单元 |
| Backend EXU/LSU | TEST 6, 7 | 访存单元 |
| Backend ROB | TEST 4 | 重排序缓冲 |
| Cache DCache | TEST 6, 7 | 数据缓存 |
| MMU/ITLB | TEST 8 | 内存管理单元 |
