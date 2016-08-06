
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fourier.h"

static int* indices;
static Complex* x;
static Complex* f;

int main(int argc, char* argv[])
{
    FILE *file;
    int i;
    int rv;

    int n = atoi(argv[1]);
    char *outputFile = argv[2];

    // create the arrays
    x = (Complex*)malloc(n * sizeof (Complex));
    f = (Complex*)malloc(n * sizeof (Complex));
    indices = (int*)malloc(n * sizeof (int));

    if(x == NULL || f == NULL || indices == NULL)
    {
        printf("cannot allocate memory for fft time series!\n");
        return -1 ;
    }

    for(i = 0; i < n; i++)
    {
        x[i].real = i;
        x[i].imag = 0 ;
    }
    radix2DitCooleyTykeyFft(n, indices, x, f) ;

    file = fopen(outputFile, "w");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", outputFile);
      exit(1);
    }

    for(i = 0; i < n; i++)
    {
        rv = fprintf(file, "%.6f\t%.6f\n", f[i].real, f[i].imag);
        if(rv < 0) {
          printf("ERROR: Unable to write to file `%s'.\n", outputFile);
          fclose(file);
          exit(1);
        }
    }

    rv = fclose(file);
    if(rv != 0) {
      printf("ERROR: Unable to close file `%s'.\n", outputFile);
      exit(1);
    }

    return 0 ;
}