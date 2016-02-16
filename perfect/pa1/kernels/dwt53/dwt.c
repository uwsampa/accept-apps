/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/pa1/kernels/dwt53/src/dwt.c $
 * $Id: dwt.c 8546 2014-04-02 21:36:22Z tallent $
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

/**************************/
/***    UNCLASSIFIED    ***/
/**************************/

/***

ALL SOURCE CODE PRESENT IN THIS FILE IS UNCLASSIFIED AND IS
BEING PROVIDED IN SUPPORT OF THE DARPA PERFECT PROGRAM.

THIS CODE IS PROVIDED AS-IS WITH NO WARRANTY, EXPRESSED, IMPLIED,
OR OTHERWISE INFERRED. USE AND SUITABILITY FOR ANY PARTICULAR
APPLICATION IS SOLELY THE RESPONSIBILITY OF THE IMPLEMENTER.
NO CLAIM OF SUITABILITY FOR ANY APPLICATION IS MADE.
USE OF THIS CODE FOR ANY APPLICATION RELEASES THE AUTHOR
AND THE US GOVT OF ANY AND ALL LIABILITY.

THE POINT OF CONTACT FOR QUESTIONS REGARDING THIS SOFTWARE IS:

  US ARMY RDECOM CERDEC NVESD, RDER-NVS-SI (JOHN HODAPP,
  john.hodapp@us.army.mil), 10221 BURBECK RD, FORT BELVOIR,
  VA 22060-5806

THIS HEADER SHALL REMAIN PART OF ALL SOURCE CODE FILES.

***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <enerc.h>

#include "xmalloc.h"
#include "dwt53.h"


int
dwt53 (APPROX algPixel_t *data, int nrows, int ncols)
{
  int err = 0;
  APPROX algPixel_t *data2 = (algPixel_t *)calloc(nrows * ncols, sizeof(algPixel_t));
  if (!data2)
  {
    fprintf(stderr, "File %s, Line %d, Memory Allocation Error", __FILE__, __LINE__);
    return -1;
  }

  /* First do all rows; This function will transpose the data
   * as it performs its final shuffling
   */

  err = dwt53_row_transpose(data, data2, nrows, ncols);

  /* We next do all the columns (they are now the rows) */

  err = dwt53_row_transpose(data2, data, ncols, nrows);

  free(data2);

  return err;
}


int
dwt53_row_transpose (APPROX algPixel_t *data, APPROX algPixel_t *data2, int nrows, int ncols)
{
  int i, j;
  int cur;

  for (i = 0; i < nrows; i++)
  {
    /* Predict the odd pixels using linear interpolation of the even pixels */
    for (j = 1; j < ncols - 1; j += 2)
    {
      cur = i * ncols + j;
#ifdef USE_SHIFT
      data[cur] -= (data[cur - 1] + data[cur + 1]) >> 1;
#else
      data[cur] -= (algPixel_t)(0.5 * (data[cur - 1] + data[cur + 1]));
#endif
    }
    /* The last odd pixel only has its left neighboring even pixel */
    cur = i * ncols + ncols - 1;
    data[cur] -= data[cur - 1];

    /* Update the even pixels using the odd pixels
     * to preserve the mean value of the pixels
     */
    for (j = 2; j < ncols; j += 2)
    {
      cur = i * ncols + j;
#ifdef USE_SHIFT
      data[cur] += (data[cur - 1] + data[cur + 1]) >> 2;
#else
      data[cur] += (algPixel_t)(0.25 * (data[cur - 1] + data[cur + 1]));
#endif
    }
    /* The first even pixel only has its right neighboring odd pixel */
    cur = i * ncols + 0;
#ifdef USE_SHIFT
    data[cur] += data[cur + 1] >> 1;
#else
    data[cur] += (algPixel_t)(0.5 * data[cur + 1]);
#endif

    /* Now rearrange the data putting the low
     * frequency components at the front and the
     * high frequency components at the back,
     * transposing the data at the same time
     */

    for (j = 0; j < ncols / 2; j++)
    {
      data2[j * nrows + i] = data[i * ncols + 2 * j];
      data2[(j + ncols / 2)* nrows + i] = data[i * ncols + 2 * j + 1];
    }
  }

  return 0;
}


int dwt53_inverse(APPROX algPixel_t *data, int nrows, int ncols)
{
  int err = 0;
  APPROX algPixel_t *data2 = (algPixel_t *)calloc(nrows * ncols, sizeof(algPixel_t));
  if (!data2)
  {
    perror("Could not allocate temp space for dwt53_inverse op");
    return -1;
  }

  err = dwt53_row_transpose_inverse(data, data2, ncols, nrows);
  err = dwt53_row_transpose_inverse(data2, data, nrows, ncols);

  free(data2);

  return err;
}


int dwt53_row_transpose_inverse(APPROX algPixel_t *data, APPROX algPixel_t *data2, int nrows, int ncols)
{
  int i, j, cur;
  for (i = 0; i < nrows; i++)
  {
    // Rearrange the data putting the low frequency components at the front
    // and the high frequency components at the back, transposing the data
    // at the same time
    for (j = 0; j < ncols / 2; j++)
    {
      data2[i * ncols + 2 * j] = data[j * nrows + i];
      data2[i * ncols + 2 * j + 1] = data[(j + ncols / 2) * nrows + i];
    }

    // Update the even pixels using the odd pixels
    // to preserve the mean value of the pixels
    for (j = 2; j < ncols; j += 2)
    {
        cur = i * ncols + j;
#ifdef USE_SHIFT
        data2[cur] -= ((data2[cur - 1] + data2[cur + 1]) >> 2);
#else
        data2[cur] -= (algPixel_t)(0.25 * (data2[cur - 1] + data2[cur + 1]));
#endif
      }
      //The first even pixel only has its right neighboring odd pixel
      cur = i * ncols + 0;
#ifdef USE_SHIFT
      data2[cur] -= (data2[cur + 1] >> 1);
#else
      data2[cur] -= (algPixel_t)(0.5 * data2[cur + 1]);
#endif

      // Predict the odd pixels using linear
      // interpolation of the even pixels
      for (j = 1; j < ncols - 1; j += 2)
      {
          cur = i * ncols + j;
#ifdef USE_SHIFT
          data2[cur] += ((data2[cur - 1] + data2[cur + 1]) >> 1);
#else
          data2[cur] += (algPixel_t)(0.5 * (data2[cur - 1] + data2[cur + 1]));
#endif
        }
        // The last odd pixel only has its left neighboring even pixel
        cur = i * ncols + ncols - 1;
        data2[cur] += data2[cur - 1];
    }

  return 0;
}
