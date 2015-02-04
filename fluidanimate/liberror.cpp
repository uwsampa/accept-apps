#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <ctime>
//#include <cstdlib>
#include <complex>

using namespace std;

#include "normal.hpp"


typedef unsigned long long uint64;
typedef long long int64;
#define rdtscll(val) do { \
       unsigned int __a,__d; \
       asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
       (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
} while(0)

namespace {
#define DRAM_BITS_PER_CELL  64
#define DRAM_COLS_PER_ROW   256
#define DRAM_COL_BITS       8 // log2(DRAM_COLS_PER_ROW)
#define DRAM_ROW_MASK       (~(DRAM_COLS_PER_ROW - 1))
#define DRAM_DECAY_MEAN     (42.00 * CLOCKS_PER_SEC)
#define DRAM_DECAY_SD       (5.45 * CLOCKS_PER_SEC)
#define REFRESH_PERIOD_10NS 640
#define CLOCKS_PER_10NS     (CLOCKS_PER_SEC/100000000)
#define REFRESH_PERIOD_CLKS (REFRESH_PERIOD_10NS * CLOCKS_PER_10NS)

struct dram_cell {
    uint64 lastUpdated;
    uint64 maxTime;
    uint64 flipList[DRAM_BITS_PER_CELL];
};

map<int, struct dram_cell> DRAM;

#define SEED 123456789

// Given an address and a time, go through the entire row and
// update the time of last refresh and the max decay time
void updateRow(const uint64 pAddress, const uint64 pTime)
{
    // Update all cells in the same row with the current time and max refresh decay time
    for(uint64 address = (pAddress & DRAM_ROW_MASK); address < ((pAddress & DRAM_ROW_MASK) + DRAM_COLS_PER_ROW); ++address)
    {
        uint64 timeDiff = pTime - DRAM[address].lastUpdated;
        if(timeDiff > DRAM[address].maxTime)
        {
            DRAM[address].lastUpdated = pTime;
            
            // Cannot have a decay time longer than the refresh period
            if(timeDiff > REFRESH_PERIOD_CLKS)
                DRAM[address].maxTime = REFRESH_PERIOD_CLKS;
            else
                DRAM[address].maxTime = timeDiff;
        }
    }
}

// No error actually injected
// Resets the decay for the cell written
// and updates the time of last refresh
// and max decay time for every other cell in the row
  uint64 injectErrorStore(int64 param, uint64 addr, uint64 val, char* type, uint64 instrumentation_time) {
    // Get time to write
    uint64 currentTime;
    rdtscll(currentTime);
    currentTime = currentTime - instrumentation_time;
    
    // Update the specific address
    DRAM[addr].lastUpdated = currentTime;
    DRAM[addr].maxTime = 0;
    
    // Update all other cells in the same row with the current time and max refresh decay time
    updateRow(addr, currentTime);
    return 0;
  }

// Updates the time of last refresh
// and max decay time for every cell in the row
// Returns a modified version of the passed value with potentially some
// bits flipped according to the max decay time and the ground state of the cell
  uint64 injectErrorLoad(int64 param, uint64 ret, uint64 addr, char* type, uint64 instrumentation_time) {
    // Get time to write
    uint64 currentTime;
    rdtscll(currentTime);
    currentTime = currentTime - instrumentation_time;
    
    // Update all cells in the same row with the current time and max refresh delay
    // This includes calculating the decay for the loaded cell
    updateRow(addr, currentTime);
    
    // If the cell doesn't have the flip times set, set them now
    if(DRAM[addr].flipList[0] == 0)
    {
        for(int bitCnt = 0; bitCnt < DRAM_BITS_PER_CELL; ++bitCnt)
        {
	  long long int seed = SEED;
	  DRAM[addr].flipList[bitCnt] = i8_normal_ab(DRAM_DECAY_MEAN, DRAM_DECAY_SD, seed);
        }
    }
    
    uint64 noiseyLoad = ret;
    uint64 decayTime = DRAM[addr].maxTime;
    
    // Determine the bits in the cell that flipped
    for(int bitCnt = 0; bitCnt < DRAM_BITS_PER_CELL; ++bitCnt)
    {
        // If enough time has passed for the bit, goto ground state
        if(DRAM[addr].flipList[bitCnt] <= decayTime)
        {
            // Determine the ground state for this row(writeRow & 0x1) ? 0x0 : 0xF
            int groundState = ((addr >> DRAM_COL_BITS) & 0x1) ? 0 : 1;
            groundState = groundState << bitCnt;
            
            // Set the bit to the ground state of the row
            // This may not represent a flip given the original value
            noiseyLoad = (noiseyLoad & ~(1 << bitCnt)) | groundState;
        }
    }
    
    return noiseyLoad;
  }
}

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);

  uint64 return_value = ret;
  if (opcode == std::string("store"))
    return_value = injectErrorStore(param, op1, op2, type, instrumentation_time);
  if (opcode == std::string("load"))
    return_value = injectErrorLoad(param, ret, op1, type, instrumentation_time);

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}
