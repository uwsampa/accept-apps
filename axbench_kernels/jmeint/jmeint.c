/*
 * jmeint.c
 *
 *  Created on: Apr 23, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 *              Thierry Moreau <moreau@cs.washington.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tritri.h"



int main(int argc, char* argv[]) {

    // Jmeint variables & pointers
    int i, idx;
    int n;
    float* xyz;
    int* x;

    int rv;
    FILE *file;
    // Input file
    char *inputFile = argv[1];
    // Output file
    char *outputFile = argv[2];

    //Read input data from file
    file = fopen(inputFile, "r");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", inputFile);
      exit(1);
    }
    rv = fscanf(file, "%i", &n);
    if(rv != 1) {
      printf("ERROR: Unable to read from file `%s'.\n", inputFile);
      fclose(file);
      exit(1);
    }

    // alloc space for the triangle coords
    xyz = (float*)malloc(n * 6 * 3 * sizeof (float));
    x = (int*)malloc(n * sizeof (int));
    if(xyz == NULL || x == NULL) {
        printf("Cannot allocate memory for the triangle coordinates!");
        return -1;
    }

    for( i = 0, idx = 0; i < n; i++, idx+=18)
    {
        rv = fscanf(file, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t", &(xyz[idx+0]),&(xyz[idx+1]),&(xyz[idx+2]),&(xyz[idx+3]),&(xyz[idx+4]),&(xyz[idx+5]),&(xyz[idx+6]),&(xyz[idx+7]),&(xyz[idx+8]),&(xyz[idx+9]),&(xyz[idx+10]),&(xyz[idx+11]),&(xyz[idx+12]),&(xyz[idx+13]),&(xyz[idx+14]),&(xyz[idx+15]),&(xyz[idx+16]),&(xyz[idx+17]));
        if(rv != 18) {
          printf("ERROR: Unable to read from file `%s'.\n", inputFile);
          fclose(file);
          exit(1);
        }
    }
    rv = fclose(file);
    if(rv != 0) {
      printf("ERROR: Unable to close file `%s'.\n", inputFile);
      exit(1);
    }

    printf("\n\nRunning jmeint benchmark on %u inputs\n", n);

    for (i = 0, idx=0; i < n; i++, idx+=18) {
        x[i] = tri_tri_intersect(
            xyz + idx + 0 * 3, xyz + idx + 1 * 3, xyz + idx + 2 * 3,
            xyz + idx + 3 * 3, xyz + idx + 4 * 3, xyz + idx + 5 * 3
        );
    }

    file = fopen(outputFile, "w");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", outputFile);
      exit(1);
    }
    rv = fprintf(file, "%d\n", n);
    if(rv < 0) {
      printf("ERROR: Unable to write to file `%s'.\n", outputFile);
      fclose(file);
      exit(1);
    }
    for(i=0; i < n; i ++) {
        rv = fprintf(file, "%d\n", x[i]);
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


    printf("Done\n", n);
    free(xyz);
    free(x);

    return 0;
}

