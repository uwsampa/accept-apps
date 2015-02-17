#include <map>
#include <cstdlib>
#include <cstring>

#define DRAM_BITS_PER_CELL  64
#define DRAM_COLS_PER_ROW   256
#define DRAM_COL_BITS       8 // log2(DRAM_COLS_PER_ROW)
#define DRAM_ROW_MASK       (~(DRAM_COLS_PER_ROW - 1))
#define DRAM_DECAY_MEAN     (42.00 * CLOCKS_PER_SEC)
#define DRAM_DECAY_SD       (5.45 * CLOCKS_PER_SEC)
#define REFRESH_PERIOD_10NS 640
#define CLOCKS_PER_10NS     (CLOCKS_PER_SEC/100000000)
#define REFRESH_PERIOD_CLKS (REFRESH_PERIOD_10NS * CLOCKS_PER_10NS)

typedef unsigned long long uint64;
typedef long long int64;

#define rdtscll(val) do { \
    unsigned int __a,__d; \
    asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
    (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
  } while(0)

struct dram_cell {
  uint64 lastUpdated;
  uint64 maxTime;
  uint64 flipList[DRAM_BITS_PER_CELL];

  dram_cell() : lastUpdated(0), maxTime(0) {
    memset(flipList, 0, sizeof(uint64) * DRAM_BITS_PER_CELL);
  }
};

typedef std::map<uint64, struct dram_cell> dram_type;

uint64 injectErrorStore(int64 param, uint64 addr, uint64 val, char* type, uint64 instrumentation_time, dram_type& DRAM);

uint64 injectErrorLoad(int64 param, uint64 ret, uint64 addr, char* type, uint64 instrumentation_time, dram_type& DRAM);
