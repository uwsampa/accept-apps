#include "enerj.hpp"

#include <cstring>

#define rdtscll(val) do { \
    unsigned int __a,__d; \
    asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
    (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
  } while(0)

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);

  uint64 return_value = ret;
  if (strcmp(opcode, "store") == 0)
    EnerJ::enerjStore(op1, instrumentation_time, type);
  else if (strcmp(opcode, "load") == 0)
    return_value = EnerJ::enerjLoad(op1, ret, instrumentation_time, type);
  else
    return_value = EnerJ::BinOp(param, ret);

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}

