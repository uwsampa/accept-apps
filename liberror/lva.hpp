
typedef unsigned long long uint64;
typedef long long int64;

class LVA {
    typedef union {
	double d;
	float f;
	uint64 b;
    } dbl_uint64;

    typedef struct {
	uint64 tag;
	int confidence;
	int degree;
	double LHB[4];
	int LHB_head;
    } lva_entry;

    
public:
  static uint64 lvaLoad(uint64 ld_address, uint64 ret,
      const char* type, uint64 pc);

private:
    static const uint64 max_rand;
    static lva_entry approximator[512];
    static dbl_uint64 GHB[4];
    static int GHB_head;
    static float threshold;
    static  float pHitRate;

    static void init();
    static bool isCacheHit(uint64 ld_address);
    static uint64 getHash(uint64 pc);
    static bool init_done;
};

