#include <stdio.h>
#include <stdlib.h>

#ifdef FIXED_POINT_LENGTH
int write_array_to_octave (long long int * data, unsigned int len, char * filename, char * name, int output_shift)
#else
int write_array_to_octave (float * data, unsigned int len, char * filename, char * name)
#endif
{
  FILE *fp = fopen(filename, "w");
  int i;

  fprintf(fp, "# Created by PERFECT 1D-FFT Benchmark\n");
  fprintf(fp, "# name: %s\n", name);
  fprintf(fp, "# type: complex matrix\n");
  fprintf(fp, "# rows: 1\n");
  fprintf(fp, "# columns: %d\n", len);

  for (i = 0; i < len; i++) {
    #ifdef FIXED_POINT_LENGTH
        #ifndef INTEGER_PART 
             fprintf(fp, " (%.15g, %.15g)", (((float)(data[2*i])) * (1 << output_shift))/(1<<FIXED_POINT_LENGTH), (((float)(data[2*i+1])) * (1 << output_shift))/(1<<FIXED_POINT_LENGTH));
        #else
             fprintf(fp, " (%.15g, %.15g)", (((float)(data[2*i])) * (1 << output_shift))/(1<<FIXED_POINT_LENGTH - INTEGER_PART), (((float)(data[2*i+1])) * (1 << output_shift))/(1<<FIXED_POINT_LENGTH - INTEGER_PART));
        #endif
    #else
        fprintf(fp, " (%.15g, %.15g)", data[2*i], data[2*i+1]);
    #endif    
  }
  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}

#ifdef FIXED_POINT_LENGTH
    int read_array_from_octave (long long int * data, unsigned int len, char * filename)
#else
    int read_array_from_octave (float * data, unsigned int len, char * filename)
#endif
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

    #ifdef FIXED_POINT_LENGTH
        #ifndef INTEGER_PART 
            data[i*2 + 0] = (long long)((x1*(1<<FIXED_POINT_LENGTH))+0.5); //assuming inputs are in [0, 1]
            data[i*2 + 1] = (long long)((x2*(1<<FIXED_POINT_LENGTH))+0.5);
        #else
            data[i*2 + 0] = (long long)((x1*(1<<FIXED_POINT_LENGTH - INTEGER_PART))+0.5); //assuming inputs are in [0, 1]
            data[i*2 + 1] = (long long)((x2*(1<<FIXED_POINT_LENGTH - INTEGER_PART))+0.5);
        #endif
    #else
        data[i*2 + 0] = x1;
        data[i*2 + 1] = x2;
    #endif    
    fscanf(fp, "\n");
  }

  fclose(fp);

  return 0;
}
