#include<map>

typedef unsigned long long uint64;
typedef long long int64;

class Flikker {
public:
  static void flikkerStore(uint64 address, uint64 align, uint64 cycles,
      const char* type);
  static uint64 flikkerLoad(uint64 address, uint64 ret, uint64 align,
      uint64 cycles, const char* type, int64 param);

private:
  static std::map<uint64, uint64> mem;
};

