#include "enerj.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

std::map<uint64, uint64> EnerJ::mem;
const double EnerJ::pError = 0.00000; 
const uint64 EnerJ::max_rand = -1;

namespace {
  const double processor_freq = 2792719000.0;

  //param == 1
  const double pMild = 0.000001;
  //param == 2
  const double pMedium = 0.0001;
  //param == 3
  const double pAggressive = 0.01;
  //param == 4
  const double pTest = 0.00000001;

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
    return !(addr & (align - 1ULL));
  }

  inline void flip_bit(uint64& n, int bit) {
    uint64 mask = 1ULL << bit;
    if (n & mask) n &= ~mask;
    else n |= mask;
  }
}

void EnerJ::enerjStore(uint64 address, uint64 align, uint64 cycles,
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

uint64 EnerJ::enerjLoad(uint64 address, uint64 ret, uint64 align, uint64 cycles,
    const char* type) {
  if (!isAligned(address, align)) {
    std::cerr << "Error: unaligned address in load instruction." << std::endl;
    exit(0);
  }

  int num_bytes = getNumBytes(type);

  for (int i = 0; i < num_bytes; ++i) {
    const double time_elapsed = static_cast<double>(cycles - mem[address]) /
        processor_freq;
    const double pFlip = pError * time_elapsed;

    for (int j = 0; j < 8; ++j) {
      const double rand_number = static_cast<double>(getRandom()) /
          static_cast<double>(max_rand);
      if (rand_number < pFlip) {
        std::cerr << "\nFlipping bit " << i * 8 + j;
        flip_bit(ret, i * 8 + j);
      }
    }

    mem[address] = cycles;
    address += 4U;
  }

  return ret;
}

uint64 EnerJ::BinOp(int64 param, uint64 ret) {
  static int nerr = 0;
  double rand_number = static_cast<double>(getRandom()) /
    static_cast<double>(max_rand);
  if ((param == 1 && rand_number < pMild)
      || (param == 2 && rand_number < pMedium)
      || (param == 3 && rand_number < pAggressive)
      || (param == 4 && rand_number < pTest)) {
    ++nerr;
    std::cerr << "nerr: " << nerr << std::endl;
    uint64 r = getRandom();
    memcpy(&ret, &r, sizeof(char));
  }

  return ret;
}

