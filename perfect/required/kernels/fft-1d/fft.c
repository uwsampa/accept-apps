/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/required/fft-1d/src/fft.c $
 * $Id: fft.c 8546 2014-04-02 21:36:22Z tallent $
 *
 *---------------------------------------------------------------------------
 * Part of PERFECT Benchmark Suite (hpc.pnnl.gov/projects/PERFECT/)
 *---------------------------------------------------------------------------
 *
 * Copyright ((c)) 2014, Battelle Memorial Institute
 * Copyright ((c)) 2014, Georgia Tech Research Corporation
 * All rights reserved.
 *
 * 1. Battelle Memorial Institute (hereinafter Battelle) and Georgia Tech
 *    Research Corporation (GTRC) hereby grant permission to any person
 *    or entity lawfully obtaining a copy of this software and associated
 *    documentation files (hereinafter "the Software") to redistribute
 *    and use the Software in source and binary forms, with or without
 *    modification.  Such person or entity may use, copy, modify, merge,
 *    publish, distribute, sublicense, and/or sell copies of the
 *    Software, and may permit others to do so, subject to the following
 *    conditions:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimers.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Other than as used herein, neither the name Battelle Memorial
 *      Institute nor Battelle may be used in any form whatsoever without
 *      the express written consent of Battelle.
 *
 *      Other than as used herein, neither the name Georgia Tech Research
 *      Corporation nor GTRC may not be used in any form whatsoever
 *      without the express written consent of GTRC.
 *
 *    * Redistributions of the software in any form, and publications
 *      based on work performed using the software should include the
 *      following citation as a reference:
 *
 *      Kevin Barker, Thomas Benson, Dan Campbell, David Ediger, Roberto
 *      Gioiosa, Adolfy Hoisie, Darren Kerbyson, Joseph Manzano, Andres
 *      Marquez, Leon Song, Nathan R. Tallent, and Antonino Tumeo.
 *      PERFECT (Power Efficiency Revolution For Embedded Computing
 *      Technologies) Benchmark Suite Manual. Pacific Northwest National
 *      Laboratory and Georgia Tech Research Institute, December 2013.
 *      http://hpc.pnnl.gov/projects/PERFECT/
 *
 * 2. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *    BATTELLE, GTRC, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *    OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **EndCopyright*************************************************************/

#include <enerc.h>
#include "fft-1d.h"

#ifdef FIXED_POINT_LENGTH
static unsigned int
_rev (unsigned int v)
#else
APPROX static __attribute__((always_inline)) unsigned int
_rev (unsigned int v)
#endif
{
    #ifdef FIXED_POINT_LENGTH
        unsigned int r = v;
        int s = sizeof(v) * CHAR_BIT - 1;
    #else 
        APPROX unsigned int r = v;
        APPROX int s = sizeof(v) * CHAR_BIT - 1;
    #endif
  for (v >>= 1; v; v >>= 1)
  {
    r <<= 1;
    r |= v & 1;
    s--;
  }
  r <<= s;

  return r;
}

#ifdef FIXED_POINT_LENGTH
static int *
bit_reverse (int * w, unsigned int N, unsigned int bits)
#else
static APPROX __attribute__((always_inline)) float *
bit_reverse (APPROX float * w, unsigned int N, unsigned int bits)
#endif
{
  unsigned int i;
    #ifdef FIXED_POINT_LENGTH
        unsigned int s, shift;
    #else
        APPROX unsigned int s, shift;
    #endif
  s = sizeof(i) * CHAR_BIT - 1;
  shift = s - bits + 1;

  for (i = 0; i < N; i++) {
    #ifdef FIXED_POINT_LENGTH
        unsigned int r;
        int t_real, t_imag;
    #else
        APPROX unsigned int r;
        APPROX float t_real, t_imag;
    #endif
    r = _rev (i);
    r >>= shift;

    #ifdef FIXED_POINT_LENGTH
    if (i < r) {
    #else
    if (ENDORSE(i < r)) {
    #endif
      t_real = w[2 * i];
      t_imag = w[2 * i + 1];
      w[2 * i] = w[2 * r];
      w[2 * i + 1] = w[2 * r + 1];
      w[2 * r] = t_real;
      w[2 * r + 1] = t_imag;
    }
  }

  return w;
}


#ifdef FIXED_POINT_LENGTH
int  fft (int * data, unsigned int N, unsigned int logn, int sign)
#else
int __attribute__((always_inline))
    fft (APPROX float * data, unsigned int N, unsigned int logn, int sign)
#endif
{



  unsigned int transform_length;
  unsigned int a, b, i, j, bit;
    #ifdef FIXED_POINT_LENGTH
        float theta, s, t, s2, w_real, w_imag, w_real_temp, w_imag_temp;
        int t_real, t_imag, z_real, z_imag;
    #else
        APPROX float theta, t_real, t_imag, w_real, w_imag, s, t, s2, z_real, z_imag;
    #endif
  transform_length = 1;

    #ifdef FIXED_POINT_LENGTH
      float approx_pi = M_PI;
      float approx_05 = 0.5;
      float approx_tl = (float) transform_length;
    #else
      APPROX float approx_pi = M_PI;
      APPROX float approx_05 = 0.5;
      APPROX float approx_tl = (float) transform_length;
    #endif
  /* bit reversal */
  bit_reverse (data, N, logn);

  /* calculation */
  for (bit = 0; bit < logn; bit++) {
    w_real = 1.0;
    w_imag = 0.0;

    theta = sign * approx_pi / approx_tl;
    s = sin (theta);
    t = sin (theta * approx_05);
    s2 = 2.0 * t * t;

    //printf("theta, s, t, s2 = %f, %f, %f, %f\n", theta, s, t, s2);

    for (a = 0; a < transform_length; a++) {
      for (b = 0; b < N; b += 2 * transform_length) {
        i = b + a;
        j = b + a + transform_length;
        
        //printf("data[%d], data[%d] = %d, %d\n", 2*j, 2*j+1, data[2*j], data[2*j+1]);

        z_real = data[2*j  ];
        z_imag = data[2*j+1];

        t_real = w_real * z_real - w_imag * z_imag;
        t_imag = w_real * z_imag + w_imag * z_real;
        
        //printf("t_real, t_imag = %d, %d\n", t_real, t_imag);

        /* write the result */
        data[2*j  ]  = data[2*i  ] - t_real;
        data[2*j+1]  = data[2*i+1] - t_imag;
        data[2*i  ] += t_real;
        data[2*i+1] += t_imag;
      }

      /* adjust w */
        #ifdef FIXED_POINT_LENGTH
            //t_real = w_real - ((int)(s*pow(2,FIXED_POINT_LENGTH)) * w_imag + (int)(s2*pow(2,FIXED_POINT_LENGTH)) * w_real);
            //t_imag = w_imag + ((int)(s*pow(2,FIXED_POINT_LENGTH)) * w_real - (int)(s2*pow(2,FIXED_POINT_LENGTH)) * w_imag);
            w_real_temp = w_real - (s * w_imag + s2 * w_real);
            w_imag_temp = w_imag + (s * w_real - s2 * w_imag);
            w_real = w_real_temp;
            w_imag = w_imag_temp;
            //printf("float(w_real), float(w_imag) = %f, %f, %d, %d\n", (float)w_real, (float)w_imag, w_real, w_imag);
            //printf("t_real, t_imag = %d, %d\n", t_real, t_imag);
            //printf("w_real, w_imag, s, s2, t_real, t_image = %d, %d, %f, %f, %d, %d\n", w_real, w_imag, s, s2, t_real, t_imag);
        #else
            t_real = w_real - (s * w_imag + s2 * w_real);
            t_imag = w_imag + (s * w_real - s2 * w_imag);
            w_real = t_real;
            w_imag = t_imag;
        #endif

    }

    transform_length *= 2;
    approx_tl = transform_length;
  }

  return 0;
}


