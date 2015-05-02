#include "enerj.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

std::map<uint64, uint64> EnerJ::mem;
const double EnerJ::pError = 0.000000001; 
const uint64 EnerJ::max_rand = -1;

namespace {
  const double processor_freq = 2792719000.0;

  const double p1 = 0.1;
  const double p2 = 0.01;
  const double p3 = 0.001;
  const double p4 = 0.0001;
  const double p5 = 0.00001;
  const double p6 = 0.000001;
  const double p7 = 0.0000001;
  const double p8 = 0.00000001;
  const double p9 = 0.000000001;

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
    const char* type, int64 param) {
  if (!isAligned(address, align)) {
    std::cerr << "Error: unaligned address in load instruction." << std::endl;
    exit(0);
  }

  int num_bytes = getNumBytes(type);
  int nAffectedBytes = (param / 10) + 1;

  for (int i = 0; i < num_bytes; ++i) {
    if (i < nAffectedBytes) { // Only flip bits in lowest byte
      const double time_elapsed = static_cast<double>(cycles - mem[address]) /
          processor_freq;
      const double pFlip = pError * time_elapsed;

      for (int j = 0; j < 8; ++j) {
        const double rand_number = static_cast<double>(getRandom()) /
            static_cast<double>(max_rand);
        if (rand_number < pFlip)
          flip_bit(ret, i * 8 + j);
      }
    }

    mem[address] = cycles;
    address += 4U;
  }

  return ret;
}

uint64 EnerJ::BinOp(int64 param, uint64 ret) {
  double rand_number = static_cast<double>(getRandom()) /
    static_cast<double>(max_rand);
  if (   (((param % 10) == 2) && (rand_number < p2))
      || (((param % 10) == 3) && (rand_number < p3))
      || (((param % 10) == 4) && (rand_number < p4))
      || (((param % 10) == 5) && (rand_number < p5))
      || (((param % 10) == 6) && (rand_number < p6))
      || (((param % 10) == 7) && (rand_number < p7))
      || (((param % 10) == 8) && (rand_number < p8))
      || (((param % 10) == 9) && (rand_number < p9)) ) {
    uint64 r = getRandom();
    int nbytes = (param / 10) + 1;
    memcpy(&ret, &r, nbytes*sizeof(char));
  }

  return ret;
}

