#include <utility>
#include <ctime>
#include <complex>

using namespace std;

#include "approxMemory.hpp"
#include "normal.hpp"

#define SEED                123456789

// Given an address and a time, go through the entire row and
// update the time of last refresh and the max decay time
void updateRow(uint64 pAddress, uint64 pTime, dram_type& DRAM)
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
uint64 injectErrorStore(int64 param, uint64 addr, uint64 val, char* type, uint64 instrumentation_time, dram_type& DRAM)
{
  // Get time to write
  uint64 currentTime;
  rdtscll(currentTime);
  currentTime = currentTime - instrumentation_time;
  
  // Update the specific address
  DRAM[addr].lastUpdated = currentTime;
  DRAM[addr].maxTime = 0;
    
  // Update all other cells in the same row with the current time and max refresh decay time
  updateRow(addr, currentTime, DRAM);
  return 0;
}

// Updates the time of last refresh
// and max decay time for every cell in the row
// Returns a modified version of the passed value with potentially some
// bits flipped according to the max decay time and the ground state of the cell
uint64 injectErrorLoad(int64 param, uint64 ret, uint64 addr, char* type, uint64 instrumentation_time, dram_type& DRAM)
{
  // Get time to write
  uint64 currentTime;
  rdtscll(currentTime);
  currentTime = currentTime - instrumentation_time;
  
  // Update all cells in the same row with the current time and max refresh delay
  // This includes calculating the decay for the loaded cell
  updateRow(addr, currentTime, DRAM);
    
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
