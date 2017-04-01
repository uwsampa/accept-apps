#include <stdlib.h>
#include <stdio.h>
#include <enerc.h>

int main(int argc, const char** argv) {
  accept_roi_begin();
  APPROX int x = atoi(argv[1]);
  APPROX int y = x * 42;
  accept_roi_end();
  printf("%i\n", y);
  return 0;
}
