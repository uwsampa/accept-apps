#include "enerj.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

#include<ios>
#include<iomanip>
#include<string>

std::map<uint64, uint64> EnerJ::mem;
const uint64 EnerJ::max_rand = -1;

// param is a three digit number
// thousands digit enables/disables memory injection (0 = off, 1 = on)
// hundreds digit enables/disables alu/fpu injection (0 = off, 1 = on)
// tens digit is number of bytes to inject error to (1 to 8 = 1 to 8 bytes) for memory/alu
// ones digit specifies level (1 to 3) (1 = mild, 2 = med, 3 = aggr)

namespace {

  const double processor_freq = 2000000000.0;

  // sram/dram probabilities (can't differentiate, so combine)
  const double p1 = 0.000000001; // 10^-9
  const double p2 = 0.00001; // 10^-5
  const double p3 = 0.001; // 10^-3 (this actually is same for dram/sram)

  // alu probabilities
  const double ap1 = 0.000001;
  const double ap2 = 0.0001;
  const double ap3 = 0.01;
  
  // fpu mantissa
  const int fpm1 = 16;
  const int fpm2 = 8;
  const int fpm3 = 4;
  const int dpm1 = 32;
  const int dpm2 = 16;
  const int dpm3 = 8;

  inline double getPMem(int level) {
    switch(level) {
    case 1:
      return p1;
    case 2:
      return p2;
    case 3:
      return p3;
    default:
      return p1;
    }
  }

  inline double getPALU(int level) {
    switch(level) {
    case 1:
      return ap1;
    case 2:
      return ap2;
    case 3:
      return ap3;
    default:
      return ap1;
    }
  }

  inline int getMantissaF(int level) {
    switch(level) {
    case 1:
      return fpm1;
    case 2:
      return fpm2;
    case 3:
      return fpm3;
    default:
      return fpm1;
    }
  }

  inline int getMantissaD(int level) {
    switch(level) {
    case 1:
      return dpm1;
    case 2:
      return dpm2;
    case 3:
      return dpm3;
    default:
      return dpm1;
    }
  }

  inline void maskMantissa(int mantissabits, int precbits, uint64 &ret) {
    if (precbits > mantissabits || precbits < 0) return;
    uint64 mask = ~(0ULL);
    int shift = mantissabits - precbits;
    ret &= (mask << shift);
  }

  inline uint64 getRandom64() {
    static bool init = false;
    static uint64 x = 12345;
    if (!init) {
      srand(time(NULL));
      x = rand();
      init = true;
    }
    x ^= (x >> 21);
    x ^= (x << 35);
    x ^= (x >> 4);
    return x;

    /*
    srand(time(NULL));
    uint64 r = rand();
    r <<= 15;
    r ^= rand();
    r <<= 15;
    r ^= rand();
    r <<= 15;
    r ^= rand();
    r <<= 15;
    r ^= rand();
    return r;
    */
  }

  inline double getRandomProb() {
    return static_cast<double>(getRandom64())/static_cast<double>(max_rand);
    /*
    srand(time(NULL));
    return ((double)rand())/(1.0*RAND_MAX);
    */
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
                       const char* type, int64 param) {
  if ((param/1000)%10 == 1) {
    if (!isAligned(address, align)) {
      std::cerr << "Error: unaligned address in store instruction." << std::endl;
      exit(0);
    }
    /*
    std::cout << "addr: " << std::hex << address 
              << " align: " << align 
              << " cycles: " << std::dec << cycles 
              << " type: " << std::string(type) << std::endl;
    */
    int num_bytes = getNumBytes(type);
    for (int i = 0; i < num_bytes; ++i) {
      mem[address] = cycles;
      address += 1U;
    }
  }
}

uint64 EnerJ::enerjLoad(uint64 address, uint64 ret, uint64 align, uint64 cycles,
                        const char* type, int64 param) {
  if ((param/1000)%10 == 1) {
    if (!isAligned(address, align)) {
      std::cerr << "Error: unaligned address in load instruction." << std::endl;
      exit(0);
    }
    /*
    std::cout << "addr: " << std::hex << address 
              << " align: " << align 
              << " ret: " << ret 
              << " cycles: " << std::dec << cycles 
              << " type: " << std::string(type) 
              << " param: " << param << std::endl;
    */
    int level = param % 10;
    int num_bytes = getNumBytes(type);
    int bytes_req = (param / 10) % 10;
    int nAffectedBytes = bytes_req < num_bytes ? bytes_req : num_bytes;
    
    for (int i = 0; i < num_bytes; ++i) {
      if (i < nAffectedBytes) { // Only flip bits in lowest bytes
        const double time_elapsed = static_cast<double>(cycles - mem[address]) /
          processor_freq;
        const double pFlip = getPMem(level) * time_elapsed;
        
        for (int j = 0; j < 8; ++j) {
          const double rand_number = getRandomProb();
          if (rand_number < pFlip)
            flip_bit(ret, i * 8 + j);
        }
      }
      
      mem[address] = cycles;
      address += 1U;
    }
  }
  return ret;
}

uint64 EnerJ::BinOp(int64 param, uint64 ret, const char* type) {
  if ((param/100)%10 == 1) { // only execute this code if param indicates error injection requested
    double rand_number = getRandomProb();

    uint64 zero = 0ULL;
    int level = param % 10;
    if (strcmp(type,"Float")==0) {
      int mbits = getMantissaF(level); // number of bits we want to keep in Mantissa for SP FP
      maskMantissa(23,mbits,ret);
      /*
      std::cout << "Float mbits: " << mbits 
                << " oldret: " << std::hex << oldret
                << " " << *reinterpret_cast<float*>(&oldret)
                << " ret: " << std::hex << ret
                << " " << *reinterpret_cast<float*>(&ret)
                << std::endl;
      */
    } else if (strcmp(type,"Double")==0) {
      int mbits = getMantissaD(level); // number of bits we want to keep in Mantissa for DP FP
      maskMantissa(52,mbits,ret);
      /*
      std::cout << "Double mbits: " << mbits 
                << " oldret: " << std::hex << oldret
                << " " << *reinterpret_cast<double*>(&oldret)
                << " ret: " << std::hex << ret
                << " " << *reinterpret_cast<double*>(&ret)
                << std::endl;
      */
    } else {
      int num_bytes = getNumBytes(type);
      int nbytes = (param / 10) % 10;
      // enfore we don't overwrite more bytes than size of data
      nbytes = (nbytes < num_bytes) ? nbytes : num_bytes;
      double pALU = getPALU(level);
      if (rand_number < pALU) {
        uint64 r = getRandom64();
        memcpy(&ret, &r, nbytes*sizeof(char));
        /*
        std::cout << "Type: " << std::string(type) << " " << num_bytes << " "
                  << " ret: " << (int64)ret
                  << " oldret: " << (int64)oldret 
                  << " param: " << param << std::endl;
        */
      }
    }
  }
  return ret;
}
