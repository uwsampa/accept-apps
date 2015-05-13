#include<map>

typedef unsigned long long uint64;
typedef long long int64;

class FuzzyMemo {
public:
  static uint64 fuzzyMemoFPOp(int64 param, const char* type, uint64 op1, uint64 op2, uint64 ret);

private:
  static std::map<uint64, uint64> table;
};

