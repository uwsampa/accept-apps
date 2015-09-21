/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/lucas-kanade/main.c $
 * $Id: main.c 8546 2014-04-02 21:36:22Z tallent $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xmalloc.h>

#include "timer.h"
#include "xmalloc.h"
#include "octave.h"
#include "wami_lucas_kanade.h"

#if INPUT_SIZE == INPUT_SIZE_SMALL
#define M 512  /* columns */
#define N 512  /* rows */
#define GRADX "small_dx.mat"
#define GRADY "small_dy.mat"
#define SIZE "small"

#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
#define M 1024  /* columns */
#define N 1024  /* rows */
#define GRADX "medium_dx.mat"
#define GRADY "medium_dy.mat"
#define SIZE "medium"

#elif INPUT_SIZE == INPUT_SIZE_LARGE
#define M 2048  /* columns */
#define N 2048  /* rows */
#define GRADX "large_dx.mat"
#define GRADY "large_dy.mat"
#define SIZE "large"

#else
#error "Unhandled value for INPUT_SIZE"
#endif


int main (int argc, char * argv[])
{
  /* Input gradients */
  APPROX fltPixel_t * gradX;
  APPROX fltPixel_t * gradY;
  /* Warped input gradients */
  APPROX fltPixel_t * gradX_warped;
  APPROX fltPixel_t * gradY_warped;
  /* Steepest descent */
  APPROX fltPixel_t * I_steepest;
  /* Hessian */
  APPROX float * H;
  /* Parameter set p */
  APPROX float p_in[6];
  /* Warp/ pixel transformation matrix */
  APPROX float W_xp[9];

  srand (time (NULL));

  STATS_INIT ();
  PRINT_STAT_STRING ("kernel", "lucas_kanade");
  PRINT_STAT_INT ("rows", N);
  PRINT_STAT_INT ("columns", M);

  gradX = calloc (M * N, sizeof(fltPixel_t));
  gradY = calloc (M * N, sizeof(fltPixel_t));
  gradX_warped = calloc (M * N, sizeof(fltPixel_t));
  gradY_warped = calloc (M * N, sizeof(fltPixel_t));
  I_steepest = calloc (6 * M * N, sizeof(fltPixel_t));
  H = calloc (6 * 6, sizeof(float));

  if (!gradX || !gradY || !gradX_warped || !gradY_warped || !I_steepest || !H) {
    fprintf(stderr, "ERROR: Allocation failed.\n");
    free (gradX);
    free (gradY);
    free (gradX_warped);
    free (gradY_warped);
    free (I_steepest);
    free (H);
    exit(-1);
  }

  /* Load horizontal and vertical gradients of the input image */
  tic ();
  read_fltarray_from_octave (ENDORSE(gradX), N, M, GRADX);
  read_fltarray_from_octave (ENDORSE(gradY), N, M, GRADY);
  PRINT_STAT_DOUBLE ("time_load", toc ());

  /* Warp the gradient I with W(x;p) */
  tic ();
    /* p */
  p_in[0] = -0.035;      /* horizontal compression */
  p_in[1] = 0.01;        /* horizontal distortion */
  p_in[2] = -0.01;       /* vertical distortion */
  p_in[3] = -0.035;      /* vertical compression */
  p_in[4] = 5.5;         /* horizontal translation */
  p_in[5] = 5.5;         /* vertical translation */

    /* W(x;p) */
  W_xp[0] = 1.0 + p_in[0];  W_xp[1] = p_in[2];        W_xp[2] = p_in[4];
  W_xp[3] = p_in[1];        W_xp[4] = 1.0 + p_in[3];  W_xp[5] = p_in[5];
  W_xp[6] = 0.0;            W_xp[7] = 0.0;            W_xp[8] = 1.0;

  accept_roi_begin();

  warp_image (gradX, M, N, W_xp, gradX_warped);
  warp_image (gradY, M, N, W_xp, gradY_warped);
  PRINT_STAT_DOUBLE ("time_warp", toc ());

  /* Compute the steepest descent images Gradient * Jacobian */
  tic ();
  steepest_descent (gradX_warped, gradY_warped, M, N, I_steepest);
  PRINT_STAT_DOUBLE ("time_steepest_descent", toc ());

  /* Compute the Hessian matrix */
  tic ();
  hessian (I_steepest, M, N, 6, H);

  accept_roi_end();

  PRINT_STAT_DOUBLE ("time_hessian", toc ());
  #ifdef AUTOTUNER
    write_fltarray_to_octave (ENDORSE(H), 6, 6, "out.mat", "output");
  #else
    write_fltarray_to_octave (ENDORSE(H), 6, 6, "output.mat", "output_" SIZE);
  #endif //AUTOTUNER
  STATS_END ();

  free (gradX);
  free (gradY);
  free (gradX_warped);
  free (gradY_warped);
  free (I_steepest);
  free (H);

  return 0;
}





