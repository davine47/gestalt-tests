# Dual-Core Hello Test Documentation

## 测试意图

本测试验证 RISCV64 双核（SMP）环境下的基本功能：
1. 多核启动与同步
2. 多核 printf 输出
3. 多核原子操作与锁
4. XiangShan 多核架构的基本功能

## 测试架构

```
src/dual-core-hello/
├── dual-core-hello.c   # 双核测试主程序
└── Makefile             # nexus-am 编译规则

scripts/
├── build_dual-core-hello.sh  # 编译脚本
└── run_dual-core-hello.sh    # Spike 双核仿真脚本

docs/
└── dual-core-hello.md         # 本文档
```

## 测试实现

### 双核启动流程

```
Hart 0 (BSP)                    Hart 1 (AP)
────────────                    ────────────
start.S: 初始化寄存器           start.S: 初始化寄存器
  sp = stack + 0*32KB             sp = stack + 1*32KB
  → _trm_init()                  → _trm_init()
                                   wait_for(&boot_flag, 1)  // 等待 BSP 唤醒
  main_hart0()                    ...
  atomic_printf header            ...
  atomic_store(&boot_flag, 1) ──→ wakeup
  computation                     main_hart1()
  atomic_printf results           atomic_printf results
  wait_for(&done_flag, 1)         computation
  _halt(0) → tohost=1            atomic_store(&done_flag, 1) ──→ signal done
                                 _halt(0) → spin
```

### 同步机制

| 同步原语 | 用途 | 实现 |
|---------|------|------|
| `boot_flag` | Hart 0 通知 Hart 1 启动 | `atomic_store` / `wait_for` |
| `done_flag` | Hart 1 通知 Hart 0 完成 | `atomic_store` / `wait_for` |
| `print_lock` | 保护 printf 并发输出 | `lock()` / `release()` (klib atomic) |
| `tohost` | Hart 0 通知 Spike 退出 | HTIF 协议 |

### Spike 适配

与单核 hello 测试一致，底层 I/O 通过 Spike HTIF 适配：
- `_putc`: 通过 `tohost` 输出字符，HTIF busy-wait 保证原子性
- `_halt`: Hart 0 通过 `tohost` 退出，Hart 1 spin
- `_trm_init`: 跳过 UARTLITE 初始化，实现多核差异化启动

## 编译与运行

### 编译
```bash
cd gestalt-tests
bash scripts/build_dual-core-hello.sh
```

### 仿真运行
```bash
bash scripts/run_dual-core-hello.sh
```

Spike 使用 `-p2` 参数模拟 2 个处理器核心。

### 预期输出
```
========================================
  Gestalt-test: Dual-Core Hello Suite
========================================

[HART 0] Hello World from hart 0!
[HART 0] Signaling hart 1 to start...
[HART 1] Hello World from hart 1!
[HART 1] Local computation complete: product(1..10) = 3628800
[HART 0] Local computation complete: sum(0..999) = 499500
[HART 0] Waiting for hart 1 to complete...
```

## XiangShan 微架构覆盖

| 模块 | 覆盖内容 |
|------|---------|
| MPE（多核扩展） | `mhartid` 读取、多核启动同步 |
| 原子操作 | `amoswap`（CAS锁）、fence 指令 |
| 缓存一致性 | 多核共享标志位 `boot_flag`/`done_flag` |
| HTIF | 多核环境下的 tohost 输出 |
