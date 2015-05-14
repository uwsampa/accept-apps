#include<map>

typedef unsigned long long uint64;
typedef long long int64;

class EnerJ {
public:
  static void enerjStore(uint64 address, uint64 align, uint64 cycles,
                         const char* type, int64 param);
  static uint64 enerjLoad(uint64 address, uint64 ret, uint64 align,
      uint64 cycles, const char* type, int64 param);
  static  uint64 BinOp(int64 param, uint64 ret, const char* type);

private:
  static std::map<uint64, uint64> mem;
  static const uint64 max_rand;
};

