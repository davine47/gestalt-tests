// This file is AI[DeepSeek V4 Pro, high]-generated and manually verified.
#include <klib.h>
#include <klib-macros.h>

// ---------------------------------------------------------------------------
// Spike HTIF: tohost/fromhost for host-target interface
// ---------------------------------------------------------------------------
volatile uint64_t tohost   __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));

// ---------------------------------------------------------------------------
// Dual-core synchronization
// ---------------------------------------------------------------------------
volatile int boot_flag __attribute__((aligned(8))) = 0;
volatile int done_flag __attribute__((aligned(8))) = 0;

// ---------------------------------------------------------------------------
// Read current hart ID
// ---------------------------------------------------------------------------
static inline int hartid(void) {
  int id;
  asm volatile("csrr %0, mhartid" : "=r"(id));
  return id;
}

// ---------------------------------------------------------------------------
// Atomic store with fence
// ---------------------------------------------------------------------------
static inline void atomic_store(volatile int *addr, int val) {
  asm volatile("fence rw,w" ::: "memory");
  *addr = val;
  asm volatile("fence w,rw" ::: "memory");
}

// ---------------------------------------------------------------------------
// Busy-wait for flag
// ---------------------------------------------------------------------------
static inline void wait_for(volatile int *addr, int val) {
  while (*addr != val) {
    asm volatile("" ::: "memory");
  }
}

// ---------------------------------------------------------------------------
// AM layer overrides for Spike HTIF
// ---------------------------------------------------------------------------
extern char _heap_start;
extern char _pmem_end;
_Area _heap = RANGE(&_heap_start, &_pmem_end);

// AI[DeepSeek V4 Pro, high] GENERATED BEGIN
void _putc(char ch) {
  tohost = 0x0101000000000000ULL | (unsigned char)ch;
  while (tohost != 0);
}

void _halt(int code) {
  // Only hart 0 writes to tohost for exit
  if (hartid() == 0) {
    tohost = (code << 1) | 1;
  }
  while (1);
}

int main(const char *args);

void _trm_init() {
  extern const char __am_mainargs;
  int id = hartid();

  if (id == 0) {
    int ret = main(&__am_mainargs);
    // Wait for hart 1 to finish, then exit
    wait_for(&done_flag, 1);
    _halt(ret);
  } else {
    // Secondary harts: wait for boot signal from hart 0
    wait_for(&boot_flag, 1);
    int ret = main(&__am_mainargs);
    // Signal hart 0 that we're done
    atomic_store(&done_flag, 1);
    _halt(ret);
  }
}
// AI GENERATED END

// ===========================================================================
// Heavy computation workload (both harts run the same code)
// ===========================================================================
#define WORKLOAD_N 50000

// AI[DeepSeek V4 Pro, high] GENERATED BEGIN
static int heavy_compute(int start, int end) {
  // Sum of squares: exercises ALU and branch prediction
  volatile int sum = 0;
  for (int i = start; i < end; i++) {
    // Interleave different operations to prevent compiler optimization
    int x = (i * 7 + 13) % 9973;
    sum += x * x;
    // Bit operations to exercise more ALU units
    sum ^= (i & 0xFF);
  }
  return sum;
}
// AI GENERATED END

// ===========================================================================
// Main: both harts execute the same function
// ===========================================================================
int main(const char *args) {
  int id = hartid();

  // Hart 0: print header and signal hart 1
  if (id == 0) {
    atomic_printf("\n");
    atomic_printf("========================================\n");
    atomic_printf("  Gestalt-test: Dual-Core Concurrent\n");
    atomic_printf("========================================\n\n");
    atomic_printf("[HART 0] Hello World! Signaling hart 1...\n");
    atomic_store(&boot_flag, 1);
  } else {
    atomic_printf("[HART 1] Hello World! Booted by hart 0.\n");
  }

  // AI[DeepSeek V4 Pro, high] GENERATED BEGIN
  // Both harts: run identical heavy computation concurrently
  atomic_printf("[HART %d] START heavy computation (%d iterations)\n",
                id, WORKLOAD_N);

  int result = heavy_compute(0, WORKLOAD_N);

  atomic_printf("[HART %d] DONE!  result = %d\n", id, result);
  // AI GENERATED END

  return 0;
}
