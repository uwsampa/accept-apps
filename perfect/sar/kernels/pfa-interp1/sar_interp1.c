/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/sar/kernels/pfa-interp1/sar_interp1.c $
 * $Id: sar_interp1.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <assert.h>
#include <enerc.h>

#include "sar_interp1.h"

static __attribute__((always_inline)) APPROX int find_nearest_range_coord(
    APPROX double target_coord,
    APPROX double input_coord_start,
    APPROX double input_coord_spacing,
    APPROX double input_coord_spacing_inv);

static __attribute__((always_inline)) APPROX float sinc(APPROX float x)
{
    if (ENDORSE(x == 0))
    {
        return 1.0f;
    }
    else
    {
        /*
         * C89 does not support sinf(), but we would use it here if it
         * were supported.
         */
        const APPROX float arg = M_PI * x;
        return (float) sin(arg) / arg;
    }
}

void sar_interp1(
    complex (* resampled)[PFA_NOUT_RANGE],
    complex (* const data)[N_RANGE],
    const APPROX float *window,
    const APPROX double input_coords_start[N_PULSES],
    const APPROX double input_coords_spacing[N_PULSES],
    const APPROX double output_coords[PFA_NOUT_RANGE])
{
    int p, r, k, rmin, rmax, window_offset;
    complex accum;
    APPROX float sinc_arg, sinc_val, win_val;
    APPROX double input_spacing, input_start, input_spacing_inv;
    APPROX float scale_factor;

    const int PFA_N_TSINC_POINTS_PER_SIDE = (T_PFA - 1)/2;

    if (N_PULSES == 0 || N_RANGE == 0 || PFA_NOUT_RANGE == 0)
    {
        return;
    }

    assert(N_RANGE > 1 && PFA_NOUT_RANGE > 1);

    for (p = 0; p < N_PULSES; ++p)
    {
        input_start = input_coords_start[p];
        input_spacing = input_coords_spacing[p];
        input_spacing_inv = 1.0 / input_spacing;

        scale_factor = fabs(output_coords[1] - output_coords[0]) * input_spacing_inv;

        for (r = 0; r < PFA_NOUT_RANGE; ++r)
        {
            const APPROX double out_coord = output_coords[r];
            APPROX int nearest = find_nearest_range_coord(
                output_coords[r], input_start, input_spacing, input_spacing_inv);
            if (ENDORSE(nearest < 0))
            {
                resampled[p][r].re = 0.0f;
                resampled[p][r].im = 0.0f;
                continue;
            }

            /* find_nearest_range_coord should never return a value >= N_RANGE */
            assert(nearest < N_RANGE); // ACCEPT_PERMIT

            /*
             * out_coord is bounded in [nearest, nearest+1], so we check
             * which of the two input coordinates is closest.
             */
            APPROX float fabsArg1 = out_coord - (input_start + (nearest+1)*input_spacing);
            APPROX float fabsArg2 = out_coord - (input_start + (nearest)*input_spacing);
            if (ENDORSE(fabsf(fabsArg1) < fabsf(fabsArg2)))
            {
                nearest = nearest + 1;
            }

            rmin = ENDORSE(nearest) - PFA_N_TSINC_POINTS_PER_SIDE;
            if (rmin < 0) { rmin = 0; }
            rmax = ENDORSE(nearest) + PFA_N_TSINC_POINTS_PER_SIDE;
            if (rmax >= N_RANGE) { rmax = N_RANGE-1; }

            window_offset = 0;
            if (ENDORSE(nearest) - PFA_N_TSINC_POINTS_PER_SIDE < 0)
            {
                window_offset = PFA_N_TSINC_POINTS_PER_SIDE - ENDORSE(nearest);
            }

            accum.re = accum.im = 0.0f;
            for (k = rmin; k <= rmax; ++k)
            {
                win_val = window[window_offset+(k-rmin)];
                sinc_arg = (out_coord - (input_start+k*input_spacing)) * input_spacing_inv;
                sinc_val = sinc(sinc_arg);
                accum.re += sinc_val * win_val * data[p][k].re;
                accum.im += sinc_val * win_val * data[p][k].im;
            }
            resampled[p][r].re = scale_factor * accum.re;
            resampled[p][r].im = scale_factor * accum.im;
        }
    }
}

/*
 * C89 does not include the family of round() functions, so
 * we include a simple implementation of round().  This version is
 * naive in the sense that it does not work for all inputs and
 * does not honor rounding modes specified by, e.g., fesetround().
 */
static __attribute__((always_inline)) int naive_round(APPROX double x)
{
    return (int) ENDORSE(x + 0.5);
}

__attribute__((always_inline)) APPROX int find_nearest_range_coord(
    APPROX double target_coord,
    APPROX double input_coord_start,
    APPROX double input_coord_spacing,
    APPROX double input_coord_spacing_inv)
{
    /*
     * Test for the target coordinate being out-of-bounds with respect to
     * the input coordinates.
     */
    if (ENDORSE(target_coord < input_coord_start ||
        target_coord >= (input_coord_start + (N_RANGE-1)*input_coord_spacing)))
    {
        return -1;
    }

    return naive_round((target_coord - input_coord_start) * input_coord_spacing_inv);
}
