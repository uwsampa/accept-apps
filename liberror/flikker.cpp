#include "flikker.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

std::map<uint64, uint64> Flikker::mem;
const uint64 Flikker::max_rand = -1;

namespace {
  const double processor_freq = 2000000000.0;

  // probability of bit flip per second
  // 1s refresh cycle, 65 x 10^-9
  const double p1 = 0.000000065;
  // 20s refresh cycle, 400 x 10^-12
  const double p20 = 0.000000000400;

  inline uint64 getRandom() {
    static uint64 x = 12345;
    x ^= (x >> 21);
    x ^= (x << 35);
    x ^= (x >> 4);
    return x;
  }

  int getNumBytes(const char* type) {
    if (strcmp(type, "Float") == 0) return sizeof(float);
    if (strcmp(type, "Double") == 0) return sizeof(double);
    if (strcmp(type, "Int32") == 0) return 4;
    if (strcmp(type, "Int64") == 0) return 8;
    if (strcmp(type, "Int8") == 0) return 1;
    if (strcmp(type, "Int1") == 0) return 1;
    if (strcmp(type, "Int16") == 0) return 2;
    if (strcmp(type, "Half") == 0) return 2;
    return 0;
  }

  inline bool isAligned(uint64 addr, uint64 align) {
    if (align == 0) return true;
    return !(addr & (align - 1ULL));
  }

  inline void flip_bit(uint64& n, int bit) {
    uint64 mask = 1ULL << bit;
    if (n & mask) n &= ~mask;
    else n |= mask;
  }
}

void Flikker::flikkerStore(uint64 address, uint64 align, uint64 cycles,
    const char* type) {
  if (!isAligned(address, align)) {
    std::cerr << "Error: unaligned address in store instruction." << std::endl;
    exit(0);
  }

  int num_bytes = getNumBytes(type);
  for (int i = 0; i < num_bytes; ++i) {
    mem[address] = cycles;
    address += 4U;
  }
}

uint64 Flikker::flikkerLoad(uint64 address, uint64 ret, uint64 align, uint64 cycles,
    const char* type, int64 param) {
  if (!isAligned(address, align)) {
    std::cerr << "Error: unaligned address in load instruction." << std::endl;
    exit(0);
  }

  int num_bytes = getNumBytes(type);

  double pError = 0.0;
  if (param == 1)
    pError = p1;
  else if (param == 2)
    pError = p20;

  // all bytes are affected
  for (int i = 0; i < num_bytes; ++i) {
    const double time_elapsed = static_cast<double>(cycles - mem[address]) /
      processor_freq;
    const double pFlip = pError * time_elapsed;
    
    for (int j = 0; j < 8; ++j) {
      const double rand_number = static_cast<double>(getRandom()) /
        static_cast<double>(max_rand);
      if (rand_number < pFlip)
        flip_bit(ret, i * 8 + j);
    }
    
    mem[address] = cycles;
    address += 4U;
  }

  return ret;
}

