/*
 * inversek2j.cpp
 *
 *  Created on: Sep. 10 2013
 *          Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kinematics.h"

#define PI 3.141592653589

typedef struct kinematicsData_ {
    APPROX float t1;
    APPROX float t2;
    APPROX float x;
    APPROX float y;
} kinematicsData;

int main(int argc, const char* argv[])
{
    FILE *file;
    int i; // Iterator
    int n; // Number of data points
    int rv;
    char *inputFile = argv[1];
    char *outputFile = argv[2];

    APPROX float theta1, theta2;
    kinematicsData* t1t2xy;

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

    // alloc space for the thetas
    t1t2xy = (kinematicsData*)malloc(n*sizeof(kinematicsData));

    for( i = 0 ; i < n; i ++)
    {
        rv = fscanf(file, "%f %f", &(t1t2xy[i].t1), &(t1t2xy[i].t2));
        if(rv != 2) {
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

    int curr_index1 = 0;
    for( i = 0 ; i < n; i ++)
    {
        forwardk2j(t1t2xy[i].t1, t1t2xy[i].t2, &(t1t2xy[i].x), &(t1t2xy[i].y));
    }

    for( i = 0 ; i < n; i ++)
    {
        inversek2j(t1t2xy[i].x, t1t2xy[i].y, &(t1t2xy[i].t1), &(t1t2xy[i].t2));
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
        rv = fprintf(file, "%.6f\t%.6f\n", t1t2xy[i].t1, t1t2xy[i].t2);
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

    free(t1t2xy) ;

    return 0 ;
}