/*
 * convolution.c
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#include "convolution.h"
#include <math.h>
#include <stdio.h>


static APPROX float kx[] =
    { -1, -2, -1,
       0,  0,  0,
       1,  2,  1 };

static APPROX float ky[] =
    { -1, 0,  1,
      -2, 0,  2,
      -1, 0,  1 };

void sobel(APPROX Pixel* src, APPROX Pixel* dst, int y, int x, int h, int w) {

    APPROX float THR = 255.0 / sqrtf(256 * 256 + 256 * 256);

    APPROX float sx = 0;
    APPROX float sy = 0;
    APPROX float s;

    int i, j;

    for (j = 0; j < 3; j++)
        for (i = 0; i < 3; i++)
            sx += src[(y+j-1)*w+(x+i-1)] * kx[j*3+i];
            sy += src[(y+j-1)*w+(x+i-1)] * ky[j*3+i];

    // sx = convolve(src, x, y, ky);
    // sy = convolve(src, x, y, kx);

    s = sx * sx + sy * sy;

    s = sqrtf(s);

    s = ENDORSE(s > THR) ? THR : s;

    dst[y*w + x] = s;
}

