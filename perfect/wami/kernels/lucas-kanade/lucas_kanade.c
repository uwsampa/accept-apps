/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/lucas-kanade/lucas_kanade.c $
 * $Id: lucas_kanade.c 8546 2014-04-02 21:36:22Z tallent $
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <enerc.h>

#include "wami_lucas_kanade.h"


void
warp_image (APPROX fltPixel_t *Iin, int nCols, int nRows, APPROX float *W_xp, APPROX fltPixel_t *Iout)
{
  int x, y;
  APPROX float Tlocalx, Tlocaly;
  APPROX float compa0, compa1, compb0, compb1;
  int index = 0;

  compb0 = W_xp[2];
  compb1 = W_xp[5];

  for (y = 1; y <= nRows; y++) {
    compa0 = W_xp[1] * ((float) y) + compb0;
    compa1 = W_xp[4] * ((float) y) + compb1;

    for (x = 1; x <= nCols; x++) {
      Tlocalx = W_xp[0] * ((float) x) + compa0;
      Tlocaly = W_xp[3] * ((float) x) + compa1;

      Iout[index] = interpolate (Tlocalx, Tlocaly, nCols, nRows, Iin);
      index++;
    }
  }

}

void
steepest_descent (APPROX fltPixel_t *gradX_warped, APPROX fltPixel_t *gradY_warped, int nCols, int nRows, APPROX fltPixel_t *I_steepest)
{
  int k;
  int x, y;
  APPROX float Jacobian_x[6], Jacobian_y[6];
  int index, j_index;

  for (y = 0; y < nRows; y++) {
    for (x = 0; x < nCols; x++) {
      index = y * nCols + x;

      Jacobian_x[0] = (float) x;
      Jacobian_x[1] = 0.0;
      Jacobian_x[2] = (float) y;
      Jacobian_x[3] = 0.0;
      Jacobian_x[4] = 1.0;
      Jacobian_x[5] = 0.0;

      Jacobian_y[0] = 0.0;
      Jacobian_y[1] = (float) x;
      Jacobian_y[2] = 0.0;
      Jacobian_y[3] = (float) y;
      Jacobian_y[4] = 0.0;
      Jacobian_y[5] = 1.0;

      for (k = 0; k < 6; k++) {
        j_index = (6 * y * nCols) + (nCols * k) + x;
        I_steepest[j_index] = (Jacobian_x[k] * gradX_warped[index]) + (Jacobian_y[k] * gradY_warped[index]);
      }
    }
  }
}

void
hessian (APPROX fltPixel_t *I_steepest, int nCols, int nRows, int np, APPROX float *H)
{
  int i, j;
  int x, y;

  for (y = 0; y < nRows; y++) {
    for (i = 0; i < np; i++) {
      for (j = 0; j < np; j++) {
        APPROX float total = 0.0;
        for (x = 0; x < nCols; x++) {
          int index1 = (6 * y * nCols) + (nCols * i) + x;
          int index2 = (6 * y * nCols) + (nCols * j) + x;
          total += I_steepest[index1] * I_steepest[index2];
        }
        H[6*i + j] += total;
      }
    }
  }

}

