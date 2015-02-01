#include <iostream>
#include <string>

typedef unsigned long long uint64;
typedef long long int64;

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
  if (opcode == std::string("store"))
    return injectErrorStore(param, op1, op2, type);
  if (opcode == std::string("load"))
    return injectErrorLoad(param, ret, op1, type);
  return ret;
}
