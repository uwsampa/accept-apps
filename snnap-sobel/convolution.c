#include "convolution.h"
#include <math.h>
#include <stdio.h>
#include <enerc.h>

static int kx[][3] =
    { { -1, -2, -1},
      {  0, 0,  0},
      {  1, 2,  1}};

static int ky[][3] =
    { { -1, 0,  1},
      { -2, 0,  2},
      { -1, 0,  1}};

APPROX int convolve(APPROX uchar i0, APPROX uchar i1, APPROX uchar i2, APPROX uchar i3, APPROX uchar i4, APPROX uchar i5, APPROX uchar i6, APPROX uchar i7, APPROX uchar i8, int k[][3]) {
    int i;
    int j;
    APPROX int r;

    r = i0*k[0][0];
    r += i1*k[0][1];
    r += i2*k[0][2];
    r += i3*k[1][0];
    r += i4*k[1][1];
    r += i5*k[1][2];
    r += i6*k[2][0];
    r += i7*k[2][1];
    r += i8*k[2][2];
    // for (j = 0; j < 3; j++)
    //     for (i = 0; i < 3; i++)
    //         r += w[i][j] * k[j][i]; //ACCEPT_PERMIT

    return r;
}

void sobel(APPROX uchar *dst, APPROX uchar i0, APPROX uchar i1, APPROX uchar i2, APPROX uchar i3, APPROX uchar i4, APPROX uchar i5, APPROX uchar i6, APPROX uchar i7, APPROX uchar i8) {
    APPROX int sx;
    APPROX int sy;
    APPROX float s;

    sx = convolve(i0, i1, i2, i3, i4, i5, i6, i7, i8, ky);
    sy = convolve(i0, i1, i2, i3, i4, i5, i6, i7, i8, kx);

    s = sqrt(sx * sx + sy * sy) * sqrt(2);
    if ENDORSE(s >= 256)
        s = 255;

    *dst = (uchar) s;
}

