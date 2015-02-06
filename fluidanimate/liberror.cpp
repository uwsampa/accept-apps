#include <iostream>
#include <string>
#include <ctime>

#include "approxMemory.hpp"

namespace {
  //param == 1
  const double pMild = 0.000001;
  //param == 2
  const double pMedium = 0.0001;
  //param == 3
  const double pAggressive = 0.01;

  inline uint64 getRandomBitStream() {
    static uint64 x = time(0);
    x ^= (x >> 21);
    x ^= (x << 35);
    x ^= (x >> 4);
    return x;
  }

  uint64 injectErrorBinOp(int64 param, uint64 ret) {
    double rand_number = static_cast<double>(getRandomBitStream()) /
      static_cast<double>((uint64)(-1));
    if ((param == 1 && rand_number < pMild)
        || (param == 2 && rand_number < pMedium)
        || (param == 3 && rand_number < pAggressive))
      return getRandomBitStream();

    return ret;
  }
}

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);

  static dram_type DRAM;

  uint64 return_value = ret;
  if (opcode == std::string("store"))
    return_value = injectErrorStore(param, op1, op2, type, instrumentation_time,
        DRAM);
  else if (opcode == std::string("load"))
    return_value = injectErrorLoad(param, ret, op1, type, instrumentation_time,
        DRAM);
  else
    return_value = injectErrorBinOp(param, ret);

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}
