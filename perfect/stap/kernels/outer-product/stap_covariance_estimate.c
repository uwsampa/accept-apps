/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/stap/kernels/outer-product/stap_covariance_estimate.c $
 * $Id: stap_covariance_estimate.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <stdio.h>
#include <enerc.h>

#include "stap_covariance_estimate.h"

void accumulate_outer_product_lower(
    complex covariance[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    const complex snapshot[N_CHAN*TDOF],
    int dop,
    int block);

/*
 * Kernel 1: Covariance Estimation (Outer Products)
 */
void stap_compute_covariance_estimate(
    complex covariance[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const datacube)[N_DOP][N_RANGE])
{
    int block, dop, cell;
    complex snapshot[N_CHAN*TDOF];
    const size_t num_cov_elements = (TDOF*N_CHAN) * (TDOF*N_CHAN) *
        N_DOP * N_BLOCKS;

    /*
     * It is assumed for simplicity that the training block
     * size evenly divides the range swath.
     */
    assert(N_RANGE % TRAINING_BLOCK_SIZE == 0);

    memset(covariance, 0, sizeof(complex)*num_cov_elements);

    for (block = 0; block < N_BLOCKS; ++block)
    {
        for (dop = 0; dop < N_DOP; ++dop)
        {
            const int first_cell = block*TRAINING_BLOCK_SIZE;
            const int last_cell = (block+1)*TRAINING_BLOCK_SIZE-1;
            int i;
	    APPROX int j;
	    // For some reason the stores to j are preventing perforation of
	    // some of the loops, even though all are in loop headers.
	    // Is it the j = i+1 initializations?

            for (cell = first_cell; cell <= last_cell; ++cell)
            {
                /* Extract the appropriate entries from the data cube. */
                extract_snapshot(
                    snapshot,
                    datacube,
                    dop,
                    cell);

                /* Outer products are accumulated over a full block. */
                accumulate_outer_product_lower(
                    covariance,
                    snapshot,
                    dop,
                    block);
            }

            /*
             * The covariance matrices are conjugate symmetric, so
             * we copy the conjugate of the lower triangular portion
             * into the upper triangular portion.
             */
            for (i = 0; i < N_CHAN*TDOF; ++i)
            {
	        for (j = i+1; ENDORSE(j < N_CHAN*TDOF); ++j)
                {
		    const complex x = covariance[dop][block][j][i]; // ACCEPT_PERMIT
                    covariance[dop][block][i][j].re = x.re;
                    covariance[dop][block][i][j].im = -1.0f * x.im;
                }
            }

            /*
             * Normalize the covariance matrices by dividing by the
             * number of training samples.
             */
            for (i = 0; i < N_CHAN*TDOF; ++i)
            {
	        for (j = 0; ENDORSE(j < N_CHAN*TDOF); ++j)
                {
                    covariance[dop][block][i][j].re *= (1.0f/TRAINING_BLOCK_SIZE);
                    covariance[dop][block][i][j].im *= (1.0f/TRAINING_BLOCK_SIZE);
                }
            }
        }
    }
}

void accumulate_outer_product_lower(
    complex covariance[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    const complex snapshot[N_CHAN*TDOF],
    int dop,
    int block)
{
    int i, j;
    for (i = 0; i < N_CHAN*TDOF; ++i)
    {
        /* Exploit conjugate symmetry by only accumulating along
         * the diagonal and below. */
        for (j = 0; j <= i; ++j)
        {
            const complex x = cmult(snapshot[i], cconj(snapshot[j]));
            covariance[dop][block][i][j].re += x.re;
            covariance[dop][block][i][j].im += x.im;
        }
    }
}
