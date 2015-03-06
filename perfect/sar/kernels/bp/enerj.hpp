#include<map>

typedef unsigned long long uint64;
typedef long long int64;

class EnerJ {
public:
  static void enerjStore(uint64 address, uint64 align, uint64 cycles,
      const char* type);
  static uint64 enerjLoad(uint64 address, uint64 ret, uint64 align,
      uint64 cycles, const char* type);
  static  uint64 BinOp(int64 param, uint64 ret);

private:
  static std::map<uint64, uint64> mem;
  static const double pError; 
  static const uint64 max_rand;
};

