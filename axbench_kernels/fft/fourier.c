
#include "fourier.h"
#include <math.h>

void calcFftIndices(int K, int* indices)
{
    int i, j ;
    int N ;

    N = (int)log2f(K) ;

    indices[0] = 0 ;
    indices[1 << 0] = 1 << (N - (0 + 1)) ;
    for (i = 1; i < N; ++i)
    {
        for(j = (1 << i) ; j < (1 << (i + 1)); ++j)
        {
            indices[j] = indices[j - (1 << i)] + (1 << (N - (i + 1))) ;
        }
    }
}

void radix2DitCooleyTykeyFft(int K, int* indices, Complex* x, Complex* f)
{

    calcFftIndices(K, indices) ;

    int step ;
    APPROX float arg ;
    int eI ;
    int oI ;

    APPROX float fftSin;
    APPROX float fftCos;

    Complex t;
    int i ;
    int N ;
    int j ;
    int k ;

    double dataIn[1];
    double dataOut[2];

    for(i = 0, N = 1 << (i + 1); N <= K ; i++, N = 1 << (i + 1))
    {
        for(j = 0 ; j < K ; j += N)
        {
            step = N >> 1 ;
            for(k = 0; k < step ; k++)
            {
                arg = (float)k / N ;
                eI = j + k ;
                oI = j + step + k ;

                fftSinCos(&arg, &fftSin, &fftCos);

                // Non-approximate
                t =  x[indices[eI]] ;
                x[indices[eI]].real = t.real + (x[indices[oI]].real * ENDORSE(fftCos) - x[indices[oI]].imag * ENDORSE(fftSin));
                x[indices[eI]].imag = t.imag + (x[indices[oI]].imag * ENDORSE(fftCos) + x[indices[oI]].real * ENDORSE(fftSin));

                x[indices[oI]].real = t.real - (x[indices[oI]].real * ENDORSE(fftCos) - x[indices[oI]].imag * ENDORSE(fftSin));
                x[indices[oI]].imag = t.imag - (x[indices[oI]].imag * ENDORSE(fftCos) + x[indices[oI]].real * ENDORSE(fftSin));
            }
        }
    }

    for (int i = 0 ; i < K ; i++)
    {
        f[i] = x[indices[i]] ;
    }
}