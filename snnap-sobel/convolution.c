#include "convolution.h"
#include <math.h>
#include <stdio.h>
#include <enerc.h>

static float kx[][3] =
    { { -1, -2, -1},
      {  0, 0,  0},
      {  1, 2,  1}};

static float ky[][3] =
    { { -1, 0,  1},
      { -2, 0,  2},
      { -1, 0,  1}};

APPROX float convolve(APPROX float i0, APPROX float i1, APPROX float i2, APPROX float i3, APPROX float i4, APPROX float i5, APPROX float i6, APPROX float i7, APPROX float i8, float k[][3]) {
    int i;
    int j;
    APPROX float r;

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

__attribute__((annotate("npu"))) float sobel(APPROX float i0, APPROX float i1, APPROX float i2, APPROX float i3, APPROX float i4, APPROX float i5, APPROX float i6, APPROX float i7, APPROX float i8) {
    APPROX float sx;
    APPROX float sy;
    APPROX float s;

    sx = convolve(i0, i1, i2, i3, i4, i5, i6, i7, i8, ky);
    sy = convolve(i0, i1, i2, i3, i4, i5, i6, i7, i8, kx);

    s = sqrt(sx * sx + sy * sy);
    if ENDORSE((s >= (256 / sqrt(256 * 256 + 256 * 256))))
        s = 255 / sqrt(256 * 256 + 256 * 256);

    return ENDORSE(s);
}

