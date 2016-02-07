/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/stap/kernels/system-solve/stap_system_solver.c $
 * $Id: stap_system_solver.c 8546 2014-04-02 21:36:22Z tallent $
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

#include "stap_utils.h"
#include "stap_system_solver.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

static void cholesky_factorization(
    complex cholesky_factors[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const covariance)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF]);

static void forward_and_back_substitution(
    complex adaptive_weights[N_DOP][N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const cholesky_factors)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF]);

/*
 * Kernel 2: Weight Generation (Linear System Solver)
 */
void stap_system_solver(
    complex adaptive_weights[N_DOP][N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const covariance)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF],
    complex cholesky_factors[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF])
{
    /*
     * The reference implementation uses Cholesky factorization followed
     * by forward and back substitution in order to solve the linear
     * systems, but other solvers can be chosen as well.
     */
    cholesky_factorization(
        cholesky_factors,
        covariance);

    forward_and_back_substitution(
        adaptive_weights,
        cholesky_factors,
        steering_vectors);
}

static void cholesky_factorization(
    complex cholesky_factors[N_DOP][N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const covariance)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF])
{
    int k, dop, block;
    APPROX int i, j;
    complex (* R)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF] = NULL;
    APPROX float Rkk_inv, Rkk_inv_sqrt;

    /*
     * cholesky_factors is a working buffer used to factorize the
     * covariance matrices in-place.  We copy the covariance matrices
     * into cholesky_factors and give cholesky_factors the convenient
     * name R for a more succinct inner loop below.
     */
    memcpy(cholesky_factors, covariance,
        sizeof(complex)*N_DOP*N_BLOCKS*N_CHAN*TDOF*N_CHAN*TDOF);
    R = cholesky_factors;

    for (dop = 0; dop < N_DOP; ++dop)
    {
        for (block = 0; block < N_BLOCKS; ++block)
        {
            /*
             * The following Cholesky factorization notation is based
             * upon the presentation in "Numerical Linear Algebra" by
             * Trefethen and Bau, SIAM, 1997.
             */
            for (k = 0; k < N_CHAN*TDOF; ++k)
            {
                /*
                 * Hermitian positive definite matrices are assumed, but
                 * for safety we check that the diagonal is always positive.
                 */
                //assert(R[dop][block][k][k].re > 0);

                /* Diagonal entries are real-valued. */
                Rkk_inv = 1.0f / R[dop][block][k][k].re;
                Rkk_inv_sqrt = sqrt(Rkk_inv);

                for (j = k+1; ENDORSE(j < N_CHAN*TDOF); ++j)
                {
                    const complex Rkj_conj = cconj(R[dop][block][k][j]);
                    for (i = j; ENDORSE(i < N_CHAN*TDOF); ++i)
                    {
                        const complex Rki_Rkj_conj = cmult(
                            R[dop][block][k][i], Rkj_conj);
                        R[dop][block][j][i].re -= Rki_Rkj_conj.re * Rkk_inv;
                        R[dop][block][j][i].im -= Rki_Rkj_conj.im * Rkk_inv;
                    }
                }
                for (i = k; ENDORSE(i < N_CHAN*TDOF); ++i)
                {
                    R[dop][block][k][i].re *= Rkk_inv_sqrt;
                    R[dop][block][k][i].im *= Rkk_inv_sqrt;
                }
            }
            /*
             * Copy the conjugate of the upper triangular portion of R
             * into the lower triangular portion. This is not required
             * for correctness, but can help with testing and validation
             * (e.g., correctness metrics calculated over all elements
             * will not be "diluted" by trivially correct zeros in the
             * lower diagonal region).
             */
            for (i = 0; ENDORSE(i < N_CHAN*TDOF); ++i)
            {
                for (j = i+1; ENDORSE(j < N_CHAN*TDOF); ++j)
                {
                    const complex x = R[dop][block][i][j]; // ACCEPT_PERMIT
                    R[dop][block][j][i].re = x.re;
                    R[dop][block][j][i].im = -1.0f * x.im;
                }
            }
        }
    }
}

static void forward_and_back_substitution(
    complex adaptive_weights[N_DOP][N_BLOCKS][N_STEERING][N_CHAN*TDOF],
    complex (* const cholesky_factors)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF],
    complex (* const steering_vectors)[N_CHAN*TDOF])
{
    /*
     * We are solving the system R*Rx = b where upper triangular matrix R
     * is the result of Cholesky factorization.  To do so, we first apply
     * forward substitution to solve R*y = b for y and then apply back
     * substitution to solve Rx = y for x.  In this case, b and x correspond
     * to the steering vectors and adaptive weights, respectively.
     */

    complex (* R)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF] = cholesky_factors;
    complex (* x)[N_BLOCKS][N_STEERING][N_CHAN*TDOF] = adaptive_weights;
    complex (* b)[N_CHAN*TDOF] = steering_vectors;
    int dop, block, sv, i, k;
    APPROX int j;
    complex accum;

    for (dop = 0; dop < N_DOP; ++dop)
    {
        for (block = 0; block < N_BLOCKS; ++block)
        {
            for (sv = 0; sv < N_STEERING; ++sv)
            {
                /* First apply forward substitution */
                for (i = 0; i < N_CHAN*TDOF; ++i)
                {
                    APPROX const float Rii_inv = 1.0f / R[dop][block][i][i].re;
                    accum.re = accum.im = 0.0f;
                    for (j = 0; ENDORSE(j < i); ++j)
                    {
                        /*
                         * Use the conjugate of the upper triangular entries
                         * of R as the lower triangular entries.
                         */
                        const complex prod = cmult(
                            cconj(R[dop][block][j][i]), x[dop][block][sv][j]);
                        accum.re += prod.re;
                        accum.im += prod.im;
                    }
                    x[dop][block][sv][i].re = (b[sv][i].re - accum.re) * Rii_inv;
                    x[dop][block][sv][i].im = (b[sv][i].im - accum.im) * Rii_inv;
                }

                /* And now apply back substitution */
                for (j = N_CHAN*TDOF-1; ENDORSE(j >= 0); --j)
                {
                    APPROX const float Rjj_inv = 1.0f / R[dop][block][j][j].re;
                    accum.re = accum.im = 0.0f;
                    for (k = ENDORSE(j+1); k < N_CHAN*TDOF; ++k)
                    {
                        const complex prod = cmult(
                            R[dop][block][j][k], x[dop][block][sv][k]);
                        accum.re += prod.re;
                        accum.im += prod.im;
                    }
                    x[dop][block][sv][j].re =
                        (x[dop][block][sv][j].re - accum.re) * Rjj_inv;
                    x[dop][block][sv][j].im =
                        (x[dop][block][sv][j].im - accum.im) * Rjj_inv;
                }
            }
        }
    }
}
