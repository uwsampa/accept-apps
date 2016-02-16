/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/change-detection/wami_gmm.c $
 * $Id: wami_gmm.c 8546 2014-04-02 21:36:22Z tallent $
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

#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <enerc.h>

#include "wami_utils.h"
#include "wami_gmm.h"

// Row and column coefficients
#define ROW_C WAMI_GMM_IMG_NUM_COLS*WAMI_GMM_NUM_MODELS
#define COL_C WAMI_GMM_NUM_MODELS

static const float STDEV_THRESH = 2.5f;
static const float INIT_STDEV = 80.0f;
static const float alpha = 0.01f; /* Learning rate */
static const float INIT_WEIGHT = 0.01f;
static const float BACKGROUND_THRESH = 0.9f;
#define ONE_OVER_SQRT_TWO_PI (1.0f / sqrt(2.0f * M_PI))

void wami_gmm(
    u8 foreground[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS],
    APPROX float *mu,
    APPROX float *sigma,
    APPROX float *weight,
    APPROX u16 *frame)
{
    const size_t num_pixels = WAMI_GMM_IMG_NUM_ROWS * WAMI_GMM_IMG_NUM_COLS;
    APPROX int row, col, k, num_foreground = 0;

    memset(foreground, 0, sizeof(u8) * num_pixels);

    for (row = 0; ENDORSE(row < WAMI_GMM_IMG_NUM_ROWS); ++row)
    {
        for (col = 0; ENDORSE(col < WAMI_GMM_IMG_NUM_COLS); ++col)
        {
            APPROX const u16 pixel = frame[row*WAMI_GMM_IMG_NUM_COLS+col];
            APPROX int match = -1;
            APPROX float sum = 0.0f, norm = 0.0f;
            APPROX int sorted_position = 0;

            for (k = 0; ENDORSE(k < WAMI_GMM_NUM_MODELS); ++k)
            {
                /*
                 * C89 does not include fabsf(), so using the double-precision
                 * fabs() function will unnecessarily type-convert to double.
                 */
                if (ENDORSE(fabs(ENDORSE(pixel - mu[row*ROW_C+col*COL_C+k]))/sigma[row*ROW_C+col*COL_C+k] < STDEV_THRESH))
                {
                    match = k;
                    break;
                }
            }

            /* Update the weights for all models */
            for (k = 0; ENDORSE(k < WAMI_GMM_NUM_MODELS); ++k)
            {
                if (ENDORSE(k == match))
                {
                    /* A model matched, so update its corresponding weight. */
                    weight[row*ROW_C+col*COL_C+match] += alpha *
                        (1.0f - weight[row*ROW_C+col*COL_C+match]);
                }
                else
                {
                    /* Non-matching models have their weights reduced */
                    weight[row*ROW_C+col*COL_C+k] *= (1.0f - alpha);
                }
            }

            if (ENDORSE(match < 0))
            {
                /*
                 * No distribution matched; replace the least likely distribution.
                 * We keep the entries sorted by significance, so the last entry
                 * is also the least likely.  We do this after updating weights
                 * above so that the initial weight is not immediately down-weighted,
                 * although that means that one update above was wasted. That
                 * update could be avoided.
                 */
                mu[row*ROW_C+col*COL_C+WAMI_GMM_NUM_MODELS-1] = ENDORSE((float) pixel);
                sigma[row*ROW_C+col*COL_C+WAMI_GMM_NUM_MODELS-1] = INIT_STDEV;
                weight[row*ROW_C+col*COL_C+WAMI_GMM_NUM_MODELS-1] = INIT_WEIGHT;
            }

            /* Normalize weights */
            for (k = 0; ENDORSE(k < WAMI_GMM_NUM_MODELS); ++k)
            {
                sum += weight[row*ROW_C+col*COL_C+k];
            }

            // removed loop exit
            // assert(sum != 0.0f);

            norm = 1.0f / sum;
            for (k = 0; ENDORSE(k < WAMI_GMM_NUM_MODELS); ++k)
            {
                weight[row*ROW_C+col*COL_C+k] *= ENDORSE(norm);
            }

            /* Update mu and sigma for the matched distribution, if any */
            if (ENDORSE(match >= 0))
            {
                APPROX const float mu_k = mu[row*ROW_C+col*COL_C+match];
                APPROX const float sigma_k = sigma[row*ROW_C+col*COL_C+match];
                APPROX const float sigma_k_inv = 1.0f / sigma_k;
                /*
                 * C89 does not include a single-precision expf() exponential function,
                 * so we instead use the double-precision variant exp(). Same for sqrt()
                 * below.
                 */
                APPROX const float rho = alpha * (ONE_OVER_SQRT_TWO_PI * sigma_k_inv) *
                    exp( -1.0f * (pixel-mu_k)*(pixel-mu_k) / (2.0f * sigma_k * sigma_k) );
                mu[row*ROW_C+col*COL_C+match] = ENDORSE((1.0f - rho) * mu_k + rho * pixel);
                sigma[row*ROW_C+col*COL_C+match] = ENDORSE(sqrt(
                    (1.0f - rho) * sigma_k * sigma_k +
                    rho * (pixel-mu[row*ROW_C+col*COL_C+match]) * (pixel-mu[row*ROW_C+col*COL_C+match])));
                // removed loop exit
                // assert(sigma[row][col][match] > 0);
            }

            /*
             * weight and sigma for the matched (or new) distribution are the only
             * values that may have changed, so we find the correct location of that
             * new value in the sorted list.  Matches lead to more evidence, and thus
             * higher weight and lower sigma, so we only need to sort "higher".
             */
            sorted_position = 0;
            if (ENDORSE(match != 0))
            {
                APPROX const int sort_from = (match >= 0) ? match : WAMI_GMM_NUM_MODELS-1;
                APPROX const float new_significance = weight[row*ROW_C+col*COL_C+sort_from] /
                    sigma[row*ROW_C+col*COL_C+sort_from];
                APPROX float other_significance, new_mu, new_sigma, new_weight;
                for (k = sort_from-1; ENDORSE(k >= 0); --k)
                {
                    other_significance = weight[row*ROW_C+col*COL_C+k] /
                        sigma[row*ROW_C+col*COL_C+k];
                    if (ENDORSE(new_significance <= other_significance))
                    {
                        break;
                    }
                }
                if (ENDORSE(k == 0))
                {
                    if (ENDORSE(other_significance >= new_significance))
                    {
                        sorted_position = 1;
                    }
                    else
                    {
                        sorted_position = 0;
                    }
                }
                else
                {
                    sorted_position = k + 1;
                }

                new_mu = mu[row*ROW_C+col*COL_C+sort_from];
                new_sigma = sigma[row*ROW_C+col*COL_C+sort_from];
                new_weight = weight[row*ROW_C+col*COL_C+sort_from];
                for (k = sort_from; ENDORSE(k > sorted_position); --k)
                {
                    mu[row*ROW_C+col*COL_C+k] = mu[row*ROW_C+col*COL_C+k-1];
                    sigma[row*ROW_C+col*COL_C+k] = sigma[row*ROW_C+col*COL_C+k-1];
                    weight[row*ROW_C+col*COL_C+k] = weight[row*ROW_C+col*COL_C+k-1];
                }
                mu[row*ROW_C+col*COL_C+sorted_position] = ENDORSE(new_mu);
                sigma[row*ROW_C+col*COL_C+sorted_position] = ENDORSE(new_sigma);
                weight[row*ROW_C+col*COL_C+sorted_position] = ENDORSE(new_weight);
            }

            /* Now, we need to determine if this pixel is foreground or background. */
            {
                APPROX float cumsum = weight[row*ROW_C+col*COL_C+0];
                int B = 0;
                while (ENDORSE(B < WAMI_GMM_NUM_MODELS-1 && cumsum <= BACKGROUND_THRESH))
                {
                    cumsum += weight[row*ROW_C+col*COL_C+(++B)];
                }
                foreground[row][col] = ENDORSE(sorted_position > B);
                num_foreground += foreground[row][col];
            }
        }
    }
}
