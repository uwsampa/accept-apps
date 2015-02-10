/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/sar/kernels/lib/sar_params.h $
 * $Id: sar_params.h 8546 2014-04-02 21:36:22Z tallent $
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

#ifndef _SAR_PARAMS_H_
#define _SAR_PARAMS_H_

#define INPUT_SIZE_SMALL 1
#define INPUT_SIZE_MEDIUM 2
#define INPUT_SIZE_LARGE 3

#ifndef INPUT_SIZE
    #define INPUT_SIZE INPUT_SIZE_MEDIUM
#endif

/*
 * We statically define the data cube dimensions and algorithm parameters
 * in order to support any tools that may require or benefit from
 * such patterns.  The dimensions can be adjusted by specifying
 * INPUT_SIZE during compilation.
 *
 * The following parameters depend upon the input sample size:
 * 
 * N_RANGE : Number of range cells
 * N_PULSES : Number of pulses
 * BP_NPIX_X / BP_NPIX_Y : Number of output pixels for the backprojection
 *      kernel in the x and y dimension, respectively
 * PFA_NOUT_RANGE / PFA_NOUT_AZIMUTH : Number of output samples from
 *      the PFA range and azimuth interpolation kernels, respectively
 */

#if INPUT_SIZE == INPUT_SIZE_SMALL
    #define N_RANGE (512)
    #define N_PULSES (512)
    #define BP_NPIX_X (512)
    #define BP_NPIX_Y (512)
    #define PFA_NOUT_RANGE (512)
    #define PFA_NOUT_AZIMUTH (512)
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    #define N_RANGE (1024)
    #define N_PULSES (1024)
    #define BP_NPIX_X (1024)
    #define BP_NPIX_Y (1024)
    #define PFA_NOUT_RANGE (1024)
    #define PFA_NOUT_AZIMUTH (1024)
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    #define N_RANGE (2048)
    #define N_PULSES (2048)
    #define BP_NPIX_X (2048)
    #define BP_NPIX_Y (2048)
    #define PFA_NOUT_RANGE (2048)
    #define PFA_NOUT_AZIMUTH (2048)
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

/* Upsampling factor used to upsample range prior to backprojection. */
#define RANGE_UPSAMPLE_FACTOR (8)

#define N_RANGE_UPSAMPLED  (N_RANGE * RANGE_UPSAMPLE_FACTOR)

/* Number of points to use in the truncated sinc interpolation. */
#define T_PFA (13)

#endif /* _SAR_PARAMS_H_ */
