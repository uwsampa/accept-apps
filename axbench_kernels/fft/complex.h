#ifndef __COMPLEX_H__
#define __COMPLEX_H__

//EnerC
#include <enerc.h>

#define PI 3.1415926535897931

typedef struct {
   float real;
   float imag;
} Complex;

void fftSinCos(APPROX float* x, APPROX float* s, APPROX float* c);
// float abs(const Complex* x);
// float arg(const Complex* x);

#endif