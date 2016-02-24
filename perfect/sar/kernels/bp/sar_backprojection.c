/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/sar/kernels/bp/sar_backprojection.c $
 * $Id: sar_backprojection.c 8546 2014-04-02 21:36:22Z tallent $
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

#include <math.h>
#include <stdio.h>
#include <enerc.h>
#include "sar_backprojection.h"

void sar_backprojection(
    complex image[BP_NPIX_Y][BP_NPIX_X],
    complex (* const data)[N_RANGE_UPSAMPLED],
    const position platpos[N_PULSES],
    APPROX double ku,
    APPROX double R0,
    APPROX double dR,
    APPROX double dxdy,
    APPROX double z0)
{
    int p, ix, iy;
    APPROX const double dR_inv = 1.0/dR;

    #pragma omp parallel for private(iy, ix, p)
    for (iy = 0; iy < BP_NPIX_Y; ++iy)
    {
        APPROX const double iy_double = iy;
        APPROX const double py = (-BP_NPIX_Y/2.0 + 0.5 + iy_double) * dxdy;
        for (ix = 0; ix < BP_NPIX_X; ++ix)
        {
            complex accum;
            APPROX const double ix_double = ix;
            APPROX const double px = (-BP_NPIX_X/2.0 + 0.5 + ix_double) * dxdy;
            accum.re = accum.im = 0.0f;
            for (p = 0; p < N_PULSES; ++p)
            {
                /* calculate the range R from the platform to this pixel */
                APPROX const double xdiff = platpos[p].x - px;
                APPROX const double ydiff = platpos[p].y - py;
                APPROX const double zdiff = platpos[p].z - z0;
                APPROX const double sqrtArg = xdiff*xdiff + ydiff*ydiff + zdiff*zdiff;
                APPROX const double R = sqrt(sqrtArg);
                /* convert to a range bin index */
                APPROX const double bin = (R-R0)*dR_inv;
                if (ENDORSE(bin >= 0 && bin <= N_RANGE_UPSAMPLED-2))
                {
                    complex sample, matched_filter, prod;
                    /* interpolation range is [bin_floor, bin_floor+1] */
                    const int bin_floor = (int) ENDORSE(bin);
                    /* interpolation weight */
                    APPROX const float w = (float) (bin - (double) bin_floor);
                    /* linearly interpolate to obtain a sample at bin */
                    sample.re = (1.0f-w)*data[p][bin_floor].re + w*data[p][bin_floor+1].re;
                    sample.im = (1.0f-w)*data[p][bin_floor].im + w*data[p][bin_floor+1].im;
                    /* compute the complex exponential for the matched filter */
                    APPROX const double trigArg = 2.0 * ku * R;
                    matched_filter.re = cos(trigArg);
                    matched_filter.im = sin(trigArg);
                    /* scale the interpolated sample by the matched filter */
                    prod = cmult(sample, matched_filter);
                    /* accumulate this pulse's contribution into the pixel */
                    accum.re += prod.re;
                    accum.im += prod.im;
                }
            }
            image[iy][ix].re = accum.re;
            image[iy][ix].im = accum.im;
        }
    }
}
