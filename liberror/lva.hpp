#include<map>

typedef unsigned long long uint64;
typedef long long int64;

class LVA {
public:
  static uint64 lvaLoad(uint64 address, uint64 ret, uint64 align,
      uint64 cycles, const char* type, int64 param);

private:
  static std::map<uint64, uint64> mem;
  static const double pError; 
  static const uint64 max_rand;
};

