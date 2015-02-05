#include <iostream>
#include <string>
#include "approxMemory.hpp"

uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);

  static dram_type DRAM;

  uint64 return_value = ret;
  if (opcode == std::string("store"))
    return_value = injectErrorStore(param, op1, op2, type, instrumentation_time, DRAM);
  if (opcode == std::string("load"))
    return_value = injectErrorLoad(param, ret, op1, type, instrumentation_time, DRAM);

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}
