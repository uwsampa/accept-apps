#ifndef _LIBERRORUTIL_H_
#define _LIBERRORUTIL_H_

#include <cstdlib> // [s]rand()
#include <cstdint> // [u]int64_t

namespace liberrorutil {
  inline uint64_t getRandom64() {
    static bool init = false;
    static uint64_t x = 12345;
    if (!init) {
      srand(time(NULL));
      x = rand();
      init = true;
    }
    x ^= (x >> 21);
    x ^= (x << 35);
    x ^= (x >> 4);
    return x;
  }

  inline double getRandomProb() {
    return static_cast<double>(getRandom64())/static_cast<double>(max_rand);
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

  inline bool isAligned(uint64_t addr, uint64_t align) {
    if (align == 0) return true;
    return !(addr & (align - 1ULL));
  }

  inline void flip_bit(uint64_t& n, int bit) {
    uint64_t mask = 0ULL;
    (bit < sizeof(n) && bit > 0) ? mask = 1ULL << bit : return;
    n ^= mask;
    /*
    if (n & mask) n &= ~mask;
    else n |= mask;
    */
  }
}

#endif /* _LIBERROR_UTIL_H_ */
