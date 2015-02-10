/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/stap/kernels/inner-product/stap_apply_weighting.c $
 * $Id: stap_apply_weighting.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <math.h>
#include <enerc.h>

#include "stap_apply_weighting.h"
#include "stap_utils.h"

static void compute_gamma_weights(
    APPROX float gamma[N_STEERING],
    complex (* const adaptive_weights)[N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF],
    int range_block,
    int dop_index);

static complex complex_inner_product(
    const complex *lhs,
    const complex *rhs,
    int length);
    
/*
 * Kernel 3: Adaptive Weighting (Inner Products)
 */
void stap_apply_weighting(
    complex output[N_STEERING][N_DOP][N_RANGE],
    complex (* const datacube)[N_DOP][N_RANGE],
    complex (* const adaptive_weights)[N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF])
{
    APPROX float gamma[N_STEERING];
    complex snapshot[TDOF * N_CHAN], prod;
    int sv, dop, block, cell;

    for (dop = 0; dop < N_DOP; ++dop)
    {
        for (block = 0; block < N_BLOCKS; ++block)
        {
            /*
             * The gamma scalar weighting factors described in the
             * benchmark document could also be computed elsewhere
             * (e.g., inside the Linear System Solve kernel or in
             * a kernel by themselves), but the computation takes the
             * form of inner products and we thus include it in the
             * Inner Product Kernel.  This is the only use of the
             * steering vectors within this kernel.
             */
            compute_gamma_weights(
                gamma,
                adaptive_weights,
                steering_vectors,
                block,
                dop);

            for (sv = 0; sv < N_STEERING; ++sv)
            {
                const int first_cell = block*TRAINING_BLOCK_SIZE;
                const int last_cell = (block+1)*TRAINING_BLOCK_SIZE-1;
                for (cell = first_cell; cell <= last_cell; ++cell)
                {
                    extract_snapshot(
                        snapshot,
                        datacube,
                        dop,
                        cell);

                    /*
                     * Apply the adaptive weights to the snapshot vector via
                     * a complex inner product.
                     */
                    prod = complex_inner_product(             // ACCEPT_PERMIT
                        &adaptive_weights[dop][block][sv][0],
                        snapshot,
                        TDOF * N_CHAN);

                    /* Normalize */
                    output[sv][dop][cell].re = prod.re * gamma[sv];
                    output[sv][dop][cell].im = prod.im * gamma[sv];
                }
            }
        }
    }
}

static complex complex_inner_product(
    const complex *lhs,
    const complex *rhs,
    int length)
{
    complex accum;
    int i;

    accum.re = accum.im = 0.0f;
    for (i = 0; i < length; ++i)
    {
        const complex prod = cmult(
            cconj(lhs[i]), rhs[i]);
        accum.re += prod.re;
        accum.im += prod.im;
    }
    return accum; // ACCEPT_PERMIT
}

static void compute_gamma_weights(
    APPROX float gamma[N_STEERING],
    complex (* const adaptive_weights)[N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF],
    int range_block,
    int dop_index)
{
    int i, sv;
    complex accum;

    for (sv = 0; sv < N_STEERING; ++sv)
    {
        accum.re = accum.im = 0.0f;
        for (i = 0; i < N_CHAN*TDOF; ++i)
        {
            const complex prod = cmult(
                cconj(adaptive_weights[dop_index][range_block][sv][i]),
                steering_vectors[sv][i]);
            accum.re += prod.re;
            accum.im += prod.im;
        }

        /*
         * In exact arithmetic, accum should be a real positive
         * scalar and thus the imaginary component should be zero.
         * However, with limited precision that may not be the case,
         * so we take the magnitude of accum.  Also, gamma is a
         * normalization scalar and thus we take the inverse of
         * the computed inner product, w*v.
         */
        gamma[sv] = sqrt(accum.re*accum.re + accum.im*accum.im);
        if (ENDORSE(gamma[sv] > 0))
        {
            gamma[sv] = 1.0f / gamma[sv];
        }
        else
        {
            gamma[sv] = 1.0f;
        }
    }
}
