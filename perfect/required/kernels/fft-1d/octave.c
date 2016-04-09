#include <stdio.h>
#include <stdlib.h>

int write_array_to_octave (float * data, unsigned int len, char * filename, char * name)
{
  FILE *fp = fopen(filename, "w");
  int i;

  fprintf(fp, "# Created by PERFECT 1D-FFT Benchmark\n");
  fprintf(fp, "# name: %s\n", name);
  fprintf(fp, "# type: complex matrix\n");
  fprintf(fp, "# rows: 1\n");
  fprintf(fp, "# columns: %d\n", len);

  for (i = 0; i < len; i++) {
    fprintf(fp, " (%.15g, %.15g)", data[2*i], data[2*i+1]);
  }
  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}

int read_array_from_octave (float * data, unsigned int len, char * filename)
{
  FILE *fp = fopen(filename, "r");
  int i;
  char buffer[100];
  char comment;

  if (!fp) {
    fprintf(stderr, "File not found: %s\n", filename);
    exit(-1);
  }

  fscanf(fp, "%c", &comment);
  while (comment == '#') {
    fgets(buffer, 100, fp);
    fscanf(fp, "%c", &comment);
  }

  for (i = 0; i < len/2; i++) {
    float x1, x2;
    char c1, c2, c3;
    fscanf(fp, "%c %f %c %f %c", &c1, &x1, &c2, &x2, &c3);
    data[i*2 + 0] = x1;
    data[i*2 + 1] = x2;
    fscanf(fp, "\n");
  }

  fclose(fp);

  return 0;
}
