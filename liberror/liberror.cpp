#include "enerj.hpp"
#include "reducedprecfp.hpp"
#include "lva.hpp"
#include "flikker.hpp"
#include "npu.hpp"


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

#define PARAM_MASK ((int64)0x0FFFF) // low 16-bits
#define MODEL_MASK ((int64)0x0FFFF0000) // high 16-bits

/*
 * injectInst drives all of the error injection routines
 * the injection routine(s) called depend on the param input
 *
 * 
 *
 *
 */
uint64 injectInst(char* opcode, int64 param, uint64 ret, uint64 op1,
    uint64 op2, char* type) {
  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);
  static uint64 initial_time = before_time;
  int64 elapsed_time = before_time - initial_time - instrumentation_time;
  if (elapsed_time < 0) {
    std::cerr << "\nNegative elapsed time\n" << std::endl;
    exit(0);
  }
  // store original return value
  uint64 return_value = ret;

  // decode parameter bits so we can pick model and pass model parameter
  int64 model = ((param & MODEL_MASK) >> 16) & PARAM_MASK; // to be safe, re-mask the low 16-bits after shifting
  int64 model_param = (param & PARAM_MASK);
  //std::cout << "[model,param] = [" << model << "," << model_param << "]" << std::endl;

  switch(model) {

  case 0: // 0 = do nothing, otherwise known as precise execution
    break;

  case 1: // enerj
    //std::cout << "EnerJ" << std::endl;
    if (strcmp(opcode, "store") == 0)
      EnerJ::enerjStore(op1, ret, elapsed_time, type, param);
    else if (strcmp(opcode, "load") == 0)
      return_value = EnerJ::enerjLoad(op1, ret, op2, elapsed_time, type, model_param);
    else
      return_value = EnerJ::BinOp(model_param, ret, type);
    break;

  case 2: // zero low order bits FP = hierarchical fpu, reduced precision fpu
    //std::cout << "Reduced Precision FPU" << std::endl;
    // invoke for non load/store instructions with Float/Double type
    if (strcmp(opcode, "store") && strcmp(opcode, "load") && (!strcmp(type, "Float") || !strcmp(type, "Double")))
      return_value = ReducedPrecFP::FPOp(model_param, ret, type);
    break;

  case 3: // flikker
    //std::cout << "Flikker" << std::endl;
    if (strcmp(opcode, "store") == 0)
      Flikker::flikkerStore(op1, ret, elapsed_time, type);
    else if (strcmp(opcode, "load") == 0)
      return_value = Flikker::flikkerLoad(op1, ret, op2, elapsed_time, type, model_param);
    break;

  case 4: // load-value approximation
    // needs overall (approx&precise) cache simulator
    // applies to FP load instructions
    //std::cout << "[pc, opcode, type] = [" << model_param << "," << opcode << "," << type << "]" << std::endl;
    if (strcmp(opcode, "load") == 0)
      return_value = LVA::lvaLoad(op1 /*ld addr*/, ret /*true value*/, type, model_param /*pc*/);
    break;

  case 5: // fuzzy memoization for fp
    break;

  case 6: // fuzzy memoization for int
    break;

  default: // default is precise, do nothing
    break;
  }



  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

  return return_value;
}

/*
 * injectRegion drives all of the coarse error injection routines
 * the injection routine(s) called depend on the param input
 *
 * 
 *
 *
 */
void injectRegion(int64 param, int64 nargs, unsigned char* image, int im_size) {

  static uint64 instrumentation_time = 0U;
  uint64 before_time;
  rdtscll(before_time);
  static uint64 initial_time = before_time;
  int64 elapsed_time = before_time - initial_time - instrumentation_time;
  if (elapsed_time < 0) {
    std::cerr << "\nNegative elapsed time\n" << std::endl;
    exit(0);
  }

  // decode parameter bits so we can pick model and pass model parameter
  int64 model = ((param & MODEL_MASK) >> 16) & PARAM_MASK; // to be safe, re-mask the low 16-bits after shifting
  int64 model_param = (param & PARAM_MASK);

  switch(model) {

    case 0: // 0 = do nothing, otherwise known as precise execution
      break;

    case 1: // Digital NPU (ISCA2014)
      invokeDigitalNPU(model_param, image, im_size);
      break;

    case 2: // Analog NPU (ISCA2014)
      invokeAnalogNPU(model_param, image, im_size);
      break;

    default: // default is precise, do nothing
      break;
  }

  uint64 after_time;
  rdtscll(after_time);
  instrumentation_time += after_time - before_time;

}
