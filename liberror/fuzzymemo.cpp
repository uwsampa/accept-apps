#include "fuzzymemo.hpp"

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

uint64 FuzzyMemo::fuzzyMemoFPOp(int64 param, const char* type, uint64 op1, uint64 op2, uint64 ret) {
  // param tells how many bits to reduce mantissa by for xor operation
  op1 = op1 >> param;
  op2 = op2 >> param;
  uint64 idx = op1 ^ op2;

  uin64 return_value = ret;
  // fuzzy memoization: op1 xor op2 -> table lookup
  return return_value;
}

