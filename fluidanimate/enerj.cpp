#include "enerj.hpp"

#include<cstring>
#include<ctime>

std::map<uint64, uint64> EnerJ::mem;
const double EnerJ::pError = 0.00001; 
const uint64 EnerJ::max_rand = -1;

namespace {
  //param == 1
  const double pMild = 0.000001;
  //param == 2
  const double pMedium = 0.0001;
  //param == 3
  const double pAggressive = 0.01;
  //param == 4
  const double pTest = 0.0000000001;

  inline uint64 getRandom() {
    static uint64 x = time(0);
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
}

void EnerJ::enerjStore(uint64 address, uint64 cycles, const char* type) {
  int num_bytes = getNumBytes(type);
  for (int i = 0; i < num_bytes; ++i) {
    mem[address] = cycles;
    address += 4U;
  }
}

uint64 EnerJ::enerjLoad(uint64 address, uint64 ret, uint64 cycles,
    const char* type) {
  int num_bytes = getNumBytes(type);
  uint64 ret_val = 0;

  for (int i = 0; i < num_bytes; ++i) {
    const double time_elapsed = static_cast<double>(cycles - mem[address]) /
        static_cast<double>(CLOCKS_PER_SEC);
    const double pFlip = pError * time_elapsed;

    const int byte = i * 8;
    for (int j = 0; j < 8; ++j) {
      ret_val <<= 1;
      const double rand_number = static_cast<double>(getRandom()) /
          static_cast<double>(max_rand);
      if (rand_number < pFlip) ret_val |= ((ret >> (byte + j)) ^ 1U) & 1U;
    }

    mem[address] = cycles;
    address += 4U;
  }

  return ret_val;
}

uint64 EnerJ::BinOp(int64 param, uint64 ret) {
  double rand_number = static_cast<double>(getRandom()) /
    static_cast<double>(max_rand);
  if ((param == 1 && rand_number < pMild)
      || (param == 2 && rand_number < pMedium)
      || (param == 3 && rand_number < pAggressive)
      || (param == 4 && rand_number < pTest))
    return getRandom();

  return ret;
}

