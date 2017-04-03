#include <stdlib.h>
#include <stdio.h>
#include <enerc.h>

int main(int argc, const char** argv) {
  accept_roi_begin();
  APPROX int x = atoi(argv[1]);
  APPROX int y = x * 9;
  int z = atoi(argv[1]) * 9;
  accept_roi_end();

  FILE *f = fopen("output.txt", "w");
  fprintf(f, "%i\n", y);
  fclose(f);
  return 0;
}
