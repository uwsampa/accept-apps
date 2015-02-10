/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/debayer/wami_debayer.c $
 * $Id: wami_debayer.c 8546 2014-04-02 21:36:22Z tallent $
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

#include "wami_debayer.h"

#define PIXEL_MAX 65535

APPROX static u16 compute_and_clamp_pixel(
    APPROX u16 pos,
    APPROX u16 neg)
{
  if (ENDORSE(pos < neg))
    {
        return 0;
    }
    else
    {
        APPROX const u16 pixel = (pos - neg) >> 3;
        if (ENDORSE(pixel > PIXEL_MAX)) { return PIXEL_MAX; }
        else { return pixel; }
    }
}

/*
 * This version handles masks with fractional negative values. In those
 * cases truncating before subtraction does not generally yield the
 * same result as truncating after subtraction.  The negative value
 * is using weights in units of 1/16ths so that the one-half portions
 * are retained.
 */
APPROX static u16 compute_and_clamp_pixel_fractional_neg(
    APPROX u16 pos,
    APPROX u16 neg)
{
    /*
     * The positive portion is converted to u32 prior to doubling because
     * otherwise some of the weights could yield overflow. At that point,
     * all weights are effectively 16x their actual value, so combining
     * the positive and negative portions and then shifting by four bits
     * yields the equivalent of a floor() applied to the result of the
     * full precision convolution.
     */
    APPROX const u32 pos_u32 = ((u32) pos) << 1;
    APPROX const u32 neg_u32 = (u32) neg;
    if (ENDORSE(pos_u32 < neg_u32))
    {
        return 0;
    }
    else
    {
        APPROX const u16 pixel = (u16) ((pos_u32 - neg_u32) >> 4);
        if (ENDORSE(pixel > PIXEL_MAX)) { return PIXEL_MAX; }
        else { return pixel; }
    }
}

APPROX static u16 interp_G_at_RRR_or_G_at_BBB(
    u16 (* const bayer)[WAMI_DEBAYER_IMG_NUM_COLS],
    APPROX u32 row,
    APPROX u32 col)
{
    /*
     * The mask to interpolate G at R or B is:
     *
     * [0  0 -1  0  0
     *  0  0  2  0  0
     * -1  2  4  2 -1
     *  0  0  2  0  0
     *  0  0 -1  0  0]/8
     */
    APPROX const u16 pos =
         2 * bayer[row-1][col] +
         2 * bayer[row][col-1] +
         4 * bayer[row][col] +
         2 * bayer[row][col+1] +
         2 * bayer[row+1][col];
    APPROX const u16 neg =
             bayer[row][col+2] +
             bayer[row-2][col] +
             bayer[row][col-2] +
             bayer[row+2][col];

    return compute_and_clamp_pixel(pos, neg);
}

APPROX static u16 interp_R_at_GRB_or_B_at_GBR(
    u16 (* const bayer)[WAMI_DEBAYER_IMG_NUM_COLS],
    APPROX u32 row,
    APPROX u32 col)
{
    /*
     * [0  0 0.5 0  0
     *  0 -1  0 -1  0
     * -1  4  5  4 -1
     *  0 -1  0 -1  0
     *  0  0 0.5 0  0]/8;
     */
    const u16 pos =
          ((bayer[row-2][col] + bayer[row+2][col]) >> 1) +
        4 * bayer[row][col-1] +
        5 * bayer[row][col] +
        4 * bayer[row][col+1];
    const u16 neg =
            bayer[row-1][col-1] +
            bayer[row-1][col+1] +
            bayer[row][col-2] +
            bayer[row][col+2] +
            bayer[row+1][col-1] +
            bayer[row+1][col+1];

    return compute_and_clamp_pixel(pos, neg);
}
    
APPROX static u16 interp_R_at_GBR_or_B_at_GRB(
    u16 (* const bayer)[WAMI_DEBAYER_IMG_NUM_COLS],
    APPROX u32 row,
    APPROX u32 col)
{
    /*
     * [0  0 -1  0  0
     *  0 -1  4 -1  0
     * 0.5 0  5  0 0.5
     *  0 -1  4 -1  0
     *  0  0 -1 0  0]/8;
     */
    const u16 pos =
        4 * bayer[row-1][col] +
          ((bayer[row][col-2] + bayer[row][col+2]) >> 1) +
        5 * bayer[row][col] +
        4 * bayer[row+1][col];
    const u16 neg =
            bayer[row-2][col] +
            bayer[row-1][col-1] +
            bayer[row-1][col+1] +
            bayer[row+1][col-1] +
            bayer[row+1][col+1] +
            bayer[row+2][col];

    return compute_and_clamp_pixel(pos, neg);
}

APPROX static u16 interp_R_at_BBB_or_B_at_RRR(
    u16 (* const bayer)[WAMI_DEBAYER_IMG_NUM_COLS],
    APPROX u32 row,
    APPROX u32 col)
{
    /*
     * [0  0 -1.5 0  0
     *  0  2  0  2  0
     * -1.5 0  6  0 -1.5
     *  0  2  0  2  0
     *  0  0 -1.5 0  0]/8;
     */
    const u16 pos =
        2 * bayer[row-1][col-1] +
        2 * bayer[row-1][col+1] +
        6 * bayer[row][col] +
        2 * bayer[row+1][col-1] +
        2 * bayer[row+1][col+1];
    const u16 neg =
       (3 * bayer[row-2][col] +
        3 * bayer[row][col-2] +
        3 * bayer[row][col+2] +
        3 * bayer[row+2][col]);

    return compute_and_clamp_pixel_fractional_neg(pos, neg);
}
    
void wami_debayer(
    rgb_pixel debayered[WAMI_DEBAYER_IMG_NUM_ROWS-2*PAD][WAMI_DEBAYER_IMG_NUM_COLS-2*PAD],
    u16 (* const bayer)[WAMI_DEBAYER_IMG_NUM_COLS])
{
    APPROX u32 row, col;

    /*
     * Demosaic the following Bayer pattern:
     * R G ...
     * G B ...
     * ... ...
     */

    /* Copy red pixels through directly */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].r = bayer[row][col];
        }
    }

    /* Copy top-right green pixels through directly */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].g = bayer[row][col];
        }
    }

    /* Copy bottom-left green pixels through directly */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].g = bayer[row][col];
        }
    }

    /* Copy blue pixels through directly */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].b = bayer[row][col];
        }
    }

    /* Interpolate green pixels at red pixels */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].g = interp_G_at_RRR_or_G_at_BBB(
                bayer, row, col);
        }
    }

    /* Interpolate green pixels at blue pixels */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].g = interp_G_at_RRR_or_G_at_BBB(
                bayer, row, col);
        }
    }

    /* Interpolate red pixels at green pixels, red row, blue column */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].r = interp_R_at_GRB_or_B_at_GBR(
                bayer, row, col);
        }
    }

    /* Interpolate blue pixels at green pixels, blue row, red column */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].b = interp_R_at_GRB_or_B_at_GBR(
                bayer, row, col);
        }
    }

    /* Interpolate red pixels at green pixels, blue row, red column */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].r = interp_R_at_GBR_or_B_at_GRB(
                bayer, row, col);
        }
    }
 
    /* Interpolate blue pixels at green pixels, red row, blue column */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].b = interp_R_at_GBR_or_B_at_GRB(
                bayer, row, col);
        }
    }

    /* Interpolate red pixels at blue pixels, blue row, blue column */
    for (row = PAD+1; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD+1; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].r = interp_R_at_BBB_or_B_at_RRR(
                bayer, row, col);
        }
    }

    /* Interpolate blue pixels at red pixels, red row, red column */
    for (row = PAD; ENDORSE(row < WAMI_DEBAYER_IMG_NUM_ROWS-PAD); row += 2)
    {
        for (col = PAD; ENDORSE(col < WAMI_DEBAYER_IMG_NUM_COLS-PAD); col += 2)
        {
            debayered[row-PAD][col-PAD].b = interp_R_at_BBB_or_B_at_RRR(
                bayer, row, col);
        }
    }
}
