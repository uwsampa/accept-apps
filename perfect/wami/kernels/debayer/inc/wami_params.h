/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/lib/wami_params.h $
 * $Id: wami_params.h 8546 2014-04-02 21:36:22Z tallent $
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

#ifndef _WAMI_PARAMS_H_
#define _WAMI_PARAMS_H_

#include <enerc.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct _rgb_pixel
{
    APPROX u16 r, g, b;
} rgb_pixel;

#define RED_CHAN 0
#define GREEN_CHAN 1
#define BLUE_CHAN 2

#define INPUT_SIZE_SMALL 1
#define INPUT_SIZE_MEDIUM 2
#define INPUT_SIZE_LARGE 3

#ifndef INPUT_SIZE
    #define INPUT_SIZE INPUT_SIZE_MEDIUM
#endif

/*
 * WAMI_DEBAYER_PAD: The number of edge pixels clipped during the
 * debayer process due to not having enough pixels for the full
 * interpolation kernel. Other interpolations could be applied near
 * the edges, but we instead clip the image for simplicity.
 */

#if INPUT_SIZE == INPUT_SIZE_SMALL
    #define WAMI_DEBAYER_IMG_NUM_ROWS 512
    #define WAMI_DEBAYER_IMG_NUM_COLS 512
    #define WAMI_GMM_IMG_NUM_ROWS 512
    #define WAMI_GMM_IMG_NUM_COLS 512
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    #define WAMI_DEBAYER_IMG_NUM_ROWS 1024
    #define WAMI_DEBAYER_IMG_NUM_COLS 1024
    #define WAMI_GMM_IMG_NUM_ROWS 1024
    #define WAMI_GMM_IMG_NUM_COLS 1024
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    #define WAMI_DEBAYER_IMG_NUM_ROWS 2048
    #define WAMI_DEBAYER_IMG_NUM_COLS 2048
    #define WAMI_GMM_IMG_NUM_ROWS 2048
    #define WAMI_GMM_IMG_NUM_COLS 2048
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

#define WAMI_GMM_NUM_MODELS 5

#define WAMI_GMM_NUM_FRAMES 5

#define WAMI_DEBAYER_PAD 2

#endif
