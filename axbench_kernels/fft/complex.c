#include "complex.h"

#include <math.h>


void fftSinCos(APPROX float* x, APPROX float* s, APPROX float* c) {
    APPROX static float COEFF = -2.0 * PI;
    APPROX float arg = COEFF * (*x);
    *s = sinf(arg);
    *c = cosf(arg);
}

// float abs(const Complex* x) {
//     return sqrt(x->real * x->real + x->imag * x->imag);
// }

// float arg(const Complex* x) {
//     if (x->real > 0)
//         return atan(x->imag / x->real);

//     if (x->real < 0 && x->imag >= 0)
//         return atan(x->imag / x->real) + PI;

//     if (x->real < 0 && x->imag < 0)
//         return atan(x->imag / x->real) - PI;

//     if (x->real == 0 && x->imag > 0)
//         return PI / 2;

//     if (x->real == 0 && x->imag < 0)
//         return -PI / 2;

//     if (x->real == 0 && x->imag == 0)
//         return 0;

//     return 0;
// }