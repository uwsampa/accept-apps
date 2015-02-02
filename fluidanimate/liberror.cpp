#include <iostream>
#include <string>

typedef unsigned long long uint64;
typedef long long int64;
#define rdtscll(val) do { \
       unsigned int __a,__d; \
       asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
       (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
} while(0)

namespace {
  uint64 injectErrorStore(int64 param, uint64 addr, uint64 val, char* type) {
    return 0;
  }

  uint64 injectErrorLoad(int64 param, uint64 ret, uint64 addr, char* type) {
    return ret;
  }
}

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);

  uint64 return_value = ret;
  if (opcode == std::string("store"))
    return_value = injectErrorStore(param, op1, op2, type);
  if (opcode == std::string("load"))
    return_value = injectErrorLoad(param, ret, op1, type);

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}
