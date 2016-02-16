/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/sar/kernels/pfa-interp2/sar_interp2.c $
 * $Id: sar_interp2.c 8546 2014-04-02 21:36:22Z tallent $
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

#include "sar_interp2.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

static int find_nearest_azimuth_coord(
    APPROX double target_coord,
    APPROX const double *input_coords);

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
        APPROX const float arg = M_PI * x;
        return (float) sin(ENDORSE(arg)) / arg;
    }
}

void sar_interp2(
    complex (* resampled)[PFA_NOUT_RANGE],
    complex (* const data)[PFA_NOUT_RANGE],
    APPROX const float *window,
    // double (* const input_coords)[N_PULSES],
    APPROX const double *input_coords,
    APPROX const double *output_coords)
{
    int p, r, k, pmin, pmax, window_offset;
    complex accum;
    APPROX float sinc_arg, sinc_val;
    APPROX float input_spacing_avg, input_spacing_avg_inv, scale_factor;

    const int PFA_N_TSINC_POINTS_PER_SIDE = (T_PFA - 1)/2;

    if (N_PULSES == 0 || PFA_NOUT_RANGE == 0 || PFA_NOUT_AZIMUTH == 0)
    {
        return;
    }

    assert(PFA_NOUT_AZIMUTH > 1 && PFA_NOUT_RANGE > 1);

    for (r = 0; r < PFA_NOUT_RANGE; ++r)
    {
        input_spacing_avg = 0.0f;
        for (p = 0; p < N_PULSES-1; ++p)
        {
            input_spacing_avg += fabs(ENDORSE(input_coords[r*PFA_NOUT_RANGE+p+1] - input_coords[r*PFA_NOUT_RANGE+p]));
        }
        input_spacing_avg /= (N_PULSES-1);
        input_spacing_avg_inv = 1.0f / input_spacing_avg;
        scale_factor = fabs(ENDORSE(output_coords[1] - output_coords[0])) * input_spacing_avg_inv;

        for (p = 0; p < PFA_NOUT_AZIMUTH; ++p)
        {
            APPROX const double out_coord = output_coords[p];
            int nearest = find_nearest_azimuth_coord(output_coords[p], &input_coords[r*PFA_NOUT_RANGE]);
            if (nearest < 0)
            {
                resampled[p][r].re = 0.0f;
                resampled[p][r].im = 0.0f;
                continue;
            }

            /* find_nearest_azimuth_coord should never return a value >= N_PULSES */
            //assert(nearest < N_PULSES); // ACCEPT_PERMIT

            /*
             * out_coord is bounded in [nearest, nearest+1], so we check
             * which of the two input coordinates is closest.
             */
            if (fabs(ENDORSE(out_coord-input_coords[r*PFA_NOUT_RANGE+nearest+1])) <
                fabs(ENDORSE(out_coord-input_coords[r*PFA_NOUT_RANGE+nearest])))
            {
                nearest = nearest + 1;
            }

            pmin = nearest - PFA_N_TSINC_POINTS_PER_SIDE;
            if (pmin < 0) { pmin = 0; }
            pmax = nearest + PFA_N_TSINC_POINTS_PER_SIDE;
            if (pmax >= N_PULSES) { pmax = N_PULSES-1; }

            window_offset = 0;
            if (nearest - PFA_N_TSINC_POINTS_PER_SIDE < 0)
            {
                window_offset = PFA_N_TSINC_POINTS_PER_SIDE - nearest;
            }

            accum.re = accum.im = 0.0f;
            for (k = pmin; k <= pmax; ++k)
            {
                sinc_arg = (out_coord - input_coords[r*PFA_NOUT_RANGE+k]) * input_spacing_avg_inv;
                sinc_val = sinc(sinc_arg);
                accum.re += sinc_val * window[window_offset+(k-pmin)] * data[k][r].re;
                accum.im += sinc_val * window[window_offset+(k-pmin)] * data[k][r].im;
            }
           resampled[p][r].re = scale_factor * accum.re;
           resampled[p][r].im = scale_factor * accum.im;
        }
    }
}

__attribute__((always_inline)) int find_nearest_azimuth_coord(
    APPROX double target_coord,
    const double *input_coords)
{
    int left_ind, right_ind, mid_ind;
    double left_val, right_val, mid_val;

    /*
     * We assume for simplicity that the input coordinates are
     * monotonically increasing.
     */
    assert(PFA_NOUT_RANGE > 1 && input_coords[1] > input_coords[0]); // ACCEPT_PERMIT

    left_ind = 0;
    right_ind = PFA_NOUT_RANGE-1;
    mid_ind = (left_ind+right_ind)/2;
    left_val = input_coords[left_ind];
    right_val = input_coords[right_ind];
    mid_val = input_coords[mid_ind];

    if (ENDORSE(target_coord < left_val || target_coord > right_val))
    {
        return -1;
    }

    while (right_ind - left_ind > 1)
    {
        if (ENDORSE(target_coord <= mid_val))
        {
            right_ind = mid_ind;
            right_val = mid_val;
        }
        else
        {
            left_ind = mid_ind;
            left_val = mid_val;
        }
        mid_ind = (left_ind+right_ind)/2;
        mid_val = input_coords[mid_ind];
    }

    return mid_ind;
}
