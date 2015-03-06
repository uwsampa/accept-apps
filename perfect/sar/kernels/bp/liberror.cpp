#include "enerj.hpp"

#include <cstring>
#include <iostream>
#include <cstdlib>

#define rdtscll(val) do { \
    unsigned int __a,__d; \
    int tmp; \
    asm volatile("cpuid" : "=a" (tmp) : "0" (1) : "ebx", "ecx", "edx", "memory"); \
    asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
    (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
    asm volatile("cpuid" : "=a" (tmp) : "0" (1) : "ebx", "ecx", "edx", "memory"); \
  } while(0)

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  //static uint64 instrumentation_time = 0U;
  //uint64 before_time;
  //rdtscll(before_time);
  //static uint64 initial_time = before_time;
  //int64 elapsed_time = before_time - initial_time - instrumentation_time;
  //if (elapsed_time < 0) {
  //  std::cerr << "\nNegative elapsed time\n" << std::endl;
  //  exit(0);
  //}

  uint64 return_value = ret;
  //if (strcmp(opcode, "store") == 0)
  //  EnerJ::enerjStore(op1, ret, elapsed_time, type);
  //else if (strcmp(opcode, "load") == 0)
  //  return_value = EnerJ::enerjLoad(op1, ret, op2, elapsed_time, type);
  //else
    return_value = EnerJ::BinOp(param, ret);

  //uint64 after_time;
  //rdtscll(after_time);
  //instrumentation_time += after_time - before_time;

  return return_value;
}

