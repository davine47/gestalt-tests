// This file is AI[DeepSeek V4 Pro, high]-generated and manually verified.
#include <klib.h>
#include <klib-macros.h>

// ---------------------------------------------------------------------------
// Spike HTIF: define tohost/fromhost symbols for Spike simulator
// communication. Spike auto-detects these ELF symbols and uses them
// for host-target interface (HTIF).
// ---------------------------------------------------------------------------
volatile uint64_t tohost   __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));

// AI[DeepSeek V4 Pro, high] GENERATED BEGIN
// Override _trm_init for Spike: skip UARTLITE init and use HTIF.
// This prevents linking trm.o from the AM archive (which targets
// UARTLITE MMIO at 0x40600000, unsupported by Spike).
extern char _heap_start;
extern char _pmem_end;
_Area _heap = RANGE(&_heap_start, &_pmem_end);

void _putc(char ch) {
  // Dev=1 (stdout), Cmd=1 (write)
  tohost = 0x0101000000000000ULL | (unsigned char)ch;
  while (tohost != 0);
}

void _halt(int code) {
  // Spike interprets tohost with bit 0 set as halt
  tohost = (code << 1) | 1;
  while (1);
}

int main(const char *args);

void _trm_init() {
  extern const char __am_mainargs;
  int ret = main(&__am_mainargs);
  _halt(ret);
}
// AI GENERATED END

// ---------------------------------------------------------------------------
// Helper: simple assertion for bare-metal
// ---------------------------------------------------------------------------
static inline void check(int cond, const char *msg) {
  if (!cond) {
    printf("FAIL: %s\n", msg);
    _halt(1);
  }
}

// ===========================================================================
// Test 1: Basic output — verify UART / printf works
// ===========================================================================
static void test_basic_print(void) {
  printf("[TEST 1] Basic character output\n");
  printf("  Hello, XiangShan!\n");
  printf("  Printable ASCII range: ");
  for (char c = '!'; c <= '~'; c++) _putc(c);
  printf("\n");
  printf("[TEST 1] PASSED\n\n");
}

// ===========================================================================
// Test 2: Data forwarding / pipeline exercise
// XiangShan backend uses Tomasulo-like OoO with physical register file.
// Chain of dependent computations stresses the rename/issue/forward path.
// ===========================================================================
static int chain_dep(int x) {
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // Long dependency chain to stress pipeline forwarding
  int a, b, c, d, e, f, g, h;
  a = x + 1;
  b = a * 3;
  c = b - 7;
  d = c ^ 0x5A5A;
  e = d + a;
  f = e * b;
  g = f - c;
  h = g ^ d;
  return h;
  // AI GENERATED END
}

static void test_data_forwarding(void) {
  printf("[TEST 2] Data forwarding / pipeline dependency\n");
  int ref = chain_dep(42);
  // Verify repeatability
  for (int i = 0; i < 100; i++) {
    int val = chain_dep(42);
    check(val == ref, "chain_dep repeatability");
    // Stress with varying inputs
    chain_dep(i);
  }
  printf("  chain_dep(42) = %d\n", ref);
  printf("[TEST 2] PASSED\n\n");
}

// ===========================================================================
// Test 3: Independent instruction chains (ILP stress)
// Multiple independent chains should be issued in parallel by the backend.
// ===========================================================================
static void test_ilp_chains(void) {
  printf("[TEST 3] Independent instruction chains (ILP)\n");
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // Four independent chains — backend should issue them concurrently
  int a = 1, b = 2, c = 3, d = 4;
  for (int i = 0; i < 50; i++) {
    a = a * 2 + 1;
    b = b * 3 - 1;
    c = (c ^ 0x3333) + (c >> 1);
    d = (d << 2) | (d >> 30);
  }
  // AI GENERATED END
  // Results should be deterministic
  check(a == -1,          "ILP chain a");
  check(b == -1192438354, "ILP chain b");
  check(c == 8738,        "ILP chain c");
  check(d == 64,          "ILP chain d");
  printf("  a=%d b=%d c=%d d=%d\n", a, b, c, d);
  printf("[TEST 3] PASSED\n\n");
}

// ===========================================================================
// Test 4: Branch prediction — complex control flow
// XiangShan frontend has BPU (Branch Prediction Unit) + FTQ.
// Alternating patterns stress the predictor.
// ===========================================================================
static int complex_branch(int n) {
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // Mixed branch patterns: easily-predictable loops + alternating conditions
  int s = 0;
  for (int i = 0; i < n; i++) {
    if (i & 1) {
      // Odd: predictable alternating pattern
      s += i;
    } else if (i % 3 == 0) {
      s -= i;
    } else if (i % 5 == 0) {
      s += i * 2;
    } else {
      s -= i / 2;
    }
    // Second branch: harder to predict
    if ((i & 7) == 0) {
      s += 1;
    } else if ((i & 7) == 3) {
      s -= 2;
    } else {
      s += 3;
    }
  }
  // AI GENERATED END
  return s;
}

static void test_branch_prediction(void) {
  printf("[TEST 4] Branch prediction stress\n");
  int ref = complex_branch(137);
  for (int i = 0; i < 20; i++) {
    check(complex_branch(137) == ref, "complex_branch repeatability");
  }
  printf("  complex_branch(137) = %d\n", ref);
  printf("[TEST 4] PASSED\n\n");
}

// ===========================================================================
// Test 5: ICache fetch stress — instruction fetch patterns
// XiangShan frontend uses FTQ + fetch-directed instruction prefetching.
// Exercising different code regions tests ICache fill / FTQ.
// ===========================================================================
static int add_small(int a, int b) { return a + b; }
static int sub_small(int a, int b) { return a - b; }
static int mul_small(int a, int b) { return a * b; }

__attribute__((noinline))
static int call_across_regions(int n) {
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // Dispatches to functions in different code areas,
  // exercising FTQ and ICache line fills
  int acc = 0;
  for (int i = 0; i < n; i++) {
    switch (i & 3) {
      case 0: acc += add_small(i, 1); break;
      case 1: acc += sub_small(i, 1); break;
      case 2: acc += mul_small(i, 2); break;
      case 3: acc += add_small(sub_small(i, 1), mul_small(i, 2)); break;
    }
  }
  // AI GENERATED END
  return acc;
}

static void test_icache_fetch(void) {
  printf("[TEST 5] ICache fetch / FTQ stress\n");
  int ref = call_across_regions(200);
  for (int i = 0; i < 10; i++) {
    check(call_across_regions(200) == ref, "call_across_regions repeatability");
  }
  printf("  call_across_regions(200) = %d\n", ref);
  printf("[TEST 5] PASSED\n\n");
}

// ===========================================================================
// Test 6: DCache — sequential, strided, and random access
// XiangShan has L1 DCache with write-back policy.
// ===========================================================================
#define DCACHE_BUF_SIZE 4096
static int dcache_buf[DCACHE_BUF_SIZE] __attribute__((aligned(64)));

static void test_dcache_access(void) {
  printf("[TEST 6] DCache access patterns\n");

  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // 6a: Sequential write + read
  for (int i = 0; i < DCACHE_BUF_SIZE; i++)
    dcache_buf[i] = i * 3;
  for (int i = 0; i < DCACHE_BUF_SIZE; i++)
    check(dcache_buf[i] == i * 3, "dcache sequential");
  printf("  6a: Sequential access OK\n");

  // 6b: Strided access (stride 8)
  for (int i = 0; i < DCACHE_BUF_SIZE; i += 8)
    dcache_buf[i] = ~i;
  for (int i = 0; i < DCACHE_BUF_SIZE; i += 8)
    check(dcache_buf[i] == (int)~i, "dcache strided");
  // Verify untouched elements still hold old values
  for (int i = 0; i < DCACHE_BUF_SIZE; i++) {
    if (i % 8 != 0) check(dcache_buf[i] == i * 3, "dcache untouched");
  }
  printf("  6b: Strided access OK\n");

  // 6c: Pseudo-random access with Xorshift
  // Use two-pass approach: first compute expected result for each index,
  // then write and verify with non-overlapping indices
  unsigned int seed = 0xDEADBEEF;
  // First pass: write to each index (no collisions — use sequential index
  // but pseudo-random value)
  for (int i = 0; i < DCACHE_BUF_SIZE; i++) {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    dcache_buf[i] = (int)seed;
  }
  // Second pass: replay same sequence and verify
  seed = 0xDEADBEEF;
  for (int i = 0; i < DCACHE_BUF_SIZE; i++) {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    check(dcache_buf[i] == (int)seed, "dcache random");
  }
  // AI GENERATED END
  printf("  6c: Pseudo-random access OK\n");
  printf("[TEST 6] PASSED\n\n");
}

// ===========================================================================
// Test 7: Memory access sizes (byte/half/word/double)
// Exercises load/store unit different data widths.
// ===========================================================================
static void test_mem_access_sizes(void) {
  printf("[TEST 7] Memory access sizes\n");
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  volatile char    *p8  = (volatile char    *)dcache_buf;
  volatile short   *p16 = (volatile short   *)dcache_buf;
  volatile int     *p32 = (volatile int     *)dcache_buf;
  volatile int64_t *p64 = (volatile int64_t *)dcache_buf;

  // Byte access
  for (int i = 0; i < 256; i++) p8[i] = (char)(i & 0xFF);
  for (int i = 0; i < 256; i++) check(p8[i] == (char)(i & 0xFF), "byte access");

  // Half-word access
  for (int i = 0; i < 128; i++) p16[i] = (short)(i * 7);
  for (int i = 0; i < 128; i++) check(p16[i] == (short)(i * 7), "half access");

  // Word access
  for (int i = 0; i < 64; i++) p32[i] = i * 0x12345;
  for (int i = 0; i < 64; i++) check(p32[i] == i * 0x12345, "word access");

  // Double-word access (RV64)
  for (int i = 0; i < 32; i++) p64[i] = (int64_t)i * 0x123456789ABCLL;
  for (int i = 0; i < 32; i++) check(p64[i] == (int64_t)i * 0x123456789ABCLL, "dword access");
  // AI GENERATED END
  printf("[TEST 7] PASSED\n\n");
}

// ===========================================================================
// Test 8: Nested loops / ITLB & BTB stress
// XiangShan frontend has ITLB, BTB in BPU.
// ===========================================================================
static void test_nested_loops(void) {
  printf("[TEST 8] Nested loops (ITLB/BTB stress)\n");
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  int sum = 0;
  // Triple-nested loop with varying bounds
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 15; j++) {
      for (int k = 0; k < 20; k++) {
        if ((i + j + k) & 1) {
          sum += k;
        } else {
          sum -= j;
        }
      }
    }
  }
  // AI GENERATED END
  check(sum == 3750, "nested loops result");
  printf("  nested_loops sum = %d\n", sum);
  printf("[TEST 8] PASSED\n\n");
}

// ===========================================================================
// Test 9: Bit manipulation — exercise ALU sub-units
// XiangShan backend has multiple functional units (ALU, MUL, DIV, etc.)
// ===========================================================================
static void test_bit_manipulation(void) {
  printf("[TEST 9] Bit manipulation (ALU exercise)\n");
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  int x = 0x5A5A5A5A;
  int y = 0xA5A5A5A5;

  // Various ALU ops
  check((x & y) == 0x00000000, "and");
  check((x | y) == 0xFFFFFFFF, "or");
  check((x ^ y) == 0xFFFFFFFF, "xor");

  // Shifts
  check((x << 1) == (int)0xB4B4B4B4, "shift left");
  check((x >> 1) == 0x2D2D2D2D, "shift right logical");

  // Arithmetic shift (sign-extending)
  int neg = -1;
  check((neg >> 1) == -1, "arithmetic shift right");

  // Rotate-like pattern
  unsigned int ux = 0x80000001;
  check(((ux << 1) | (ux >> 31)) == 0x00000003, "rotate left 1");
  // AI GENERATED END
  printf("[TEST 9] PASSED\n\n");
}

// ===========================================================================
// Test 10: Recursion — stack and RAS (Return Address Stack) stress
// XiangShan BPU has RAS for predicting function returns.
// ===========================================================================
static int deep_recursion(int n) {
  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  if (n <= 1) return 1;
  if (n & 1)
    return n + deep_recursion(n - 1);
  else
    return n * deep_recursion(n - 1);
  // AI GENERATED END
}

static void test_recursion_ras(void) {
  printf("[TEST 10] Recursion / RAS stress\n");
  int r = deep_recursion(10);
  check(r == 12650, "deep_recursion(10)");
  printf("  deep_recursion(10) = %d\n", r);
  printf("[TEST 10] PASSED\n\n");
}

// ===========================================================================
// Main
// ===========================================================================
int main(const char *args) {
  printf("\n");
  printf("========================================\n");
  printf("  Gestalt-test: XiangShan Hello Suite\n");
  printf("========================================\n\n");

  test_basic_print();
  test_data_forwarding();
  test_ilp_chains();
  test_branch_prediction();
  test_icache_fetch();
  test_dcache_access();
  test_mem_access_sizes();
  test_nested_loops();
  test_bit_manipulation();
  test_recursion_ras();

  printf("========================================\n");
  printf("  All tests PASSED!\n");
  printf("========================================\n\n");

  return 0;
}
