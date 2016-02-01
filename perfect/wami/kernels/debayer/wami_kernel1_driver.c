/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/debayer/wami_kernel1_driver.c $
 * $Id: wami_kernel1_driver.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "wami_params.h"
#include "wami_utils.h"
#include "wami_debayer.h"

#include "timer.h"

#if INPUT_SIZE == INPUT_SIZE_SMALL
#ifdef AUTOTUNER
    static const char *output_filename = "out.bin";
    static const char *golden_output_filename = "orig.bin";
#else
    static const char *output_filename = "small_kernel1_output.bin";
    static const char *golden_output_filename = "small_golden_kernel1_output.bin";
#endif //AUTOTUNER
    static const char *input_filename = "small_kernel1_input.bin";
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    static const char *output_filename = "medium_kernel1_output.bin";
    static const char *golden_output_filename = "medium_golden_kernel1_output.bin";
    static const char *input_filename = "medium_kernel1_input.bin";
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    static const char *output_filename = "large_kernel1_output.bin";
    static const char *golden_output_filename = "large_golden_kernel1_output.bin";
    static const char *input_filename = "large_kernel1_input.bin";
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

// Let's turn this off for now since it does not tolerate approximate outputs
#define ENABLE_CORRECTNESS_CHECKING
#define WRITE_OUTPUT_TO_DISK

static const char *output_directory = ".";

int main(int argc, char **argv)
{
    u16 (*bayer)[WAMI_DEBAYER_IMG_NUM_COLS] = NULL;
    rgb_pixel (*debayer)[WAMI_DEBAYER_IMG_NUM_COLS-2*PAD] = NULL;
    char *input_directory = NULL;
#ifdef ENABLE_CORRECTNESS_CHECKING
    rgb_pixel (*gold_debayer)[WAMI_DEBAYER_IMG_NUM_COLS-2*PAD] = NULL;
#endif

    const size_t num_bayer_pixels = WAMI_DEBAYER_IMG_NUM_ROWS *
        WAMI_DEBAYER_IMG_NUM_COLS;
    const size_t num_debayer_pixels = (WAMI_DEBAYER_IMG_NUM_ROWS-2*PAD) *
        (WAMI_DEBAYER_IMG_NUM_COLS-2*PAD);

    if (argc != 2)
    {
        fprintf(stderr, "%s <directory-containing-input-files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_directory = argv[1];

    bayer = XMALLOC(sizeof(u16) * num_bayer_pixels);
    debayer = XMALLOC(sizeof(rgb_pixel) * num_debayer_pixels);
#ifdef ENABLE_CORRECTNESS_CHECKING
    gold_debayer = XMALLOC(sizeof(rgb_pixel) * num_debayer_pixels);
#endif

    read_image_file(
        (char *) bayer,
        input_filename,
        input_directory,
        sizeof(u16) * num_bayer_pixels);

    memset(debayer, 0, sizeof(u16) * num_debayer_pixels);

    printf("WAMI kernel 1 parameters:\n\n");
    printf("Input image width = %u pixels\n", WAMI_DEBAYER_IMG_NUM_COLS);
    printf("Input image height = %u pixels\n", WAMI_DEBAYER_IMG_NUM_ROWS);
    printf("Output image width = %u pixels\n", WAMI_DEBAYER_IMG_NUM_COLS-2*PAD);
    printf("Output image height = %u pixels\n", WAMI_DEBAYER_IMG_NUM_ROWS-2*PAD);

    printf("\nStarting WAMI kernel 1 (debayer).\n");
    tic();
    accept_roi_begin();
    wami_debayer(
        debayer,
        bayer);
    accept_roi_end();
    PRINT_STAT_DOUBLE("CPU time using func toc - ", toc());

#ifdef ENABLE_CORRECTNESS_CHECKING
    read_image_file(
        (char *) gold_debayer,
        golden_output_filename,
        input_directory,
        sizeof(rgb_pixel) * num_debayer_pixels);

    /*
     * An exact match is expected for the debayer kernel, so we check
     * each pixel individually and report either the first failure or
     * a success message.
     */
    {
        // // original error metric
        // int r, c, success = 1;
        // for (r = 0; success && r < WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD; ++r)
        // {
        //     for (c = 0; c < WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD; ++c)
        //     {
        //     if (ENDORSE(debayer[r][c].r != gold_debayer[r][c].r))
        //         {
        //             printf("Validation error: red pixel mismatch at row=%d, col=%d : "
        //                 "test value = %u, golden value = %u\n\n", r, c,
        //                 debayer[r][c].r, gold_debayer[r][c].r);
        //             success = 0;
        //             break;
        //         }

        //         if (ENDORSE(debayer[r][c].g != gold_debayer[r][c].g))
        //         {
        //             printf("Validation error: green pixel mismatch at row=%d, col=%d : "
        //                 "test value = %u, golden value = %u\n\n", r, c,
        //                 debayer[r][c].g, gold_debayer[r][c].g);
        //             success = 0;
        //             break;
        //         }

        //         if (ENDORSE(debayer[r][c].b != gold_debayer[r][c].b))
        //         {
        //             printf("Validation error: blue pixel mismatch at row=%d, col=%d : "
        //                 "test value = %u, golden value = %u\n\n", r, c,
        //                 debayer[r][c].b, gold_debayer[r][c].b);
        //             success = 0;
        //             break;
        //         }
        //     }
        // }
        // if (success)
        // {
        //     printf("\nValidation checks passed -- the test output matches the golden output.\n\n");
        // }

        // new error metric
        int r, c;
        double err;
        for (r = 0; r < WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD; ++r)
        {
            for (c = 0; c < WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD; ++c)
            {
                double pixel_error = 0.0;
                pixel_error += ENDORSE(((double) abs(debayer[r][c].r - gold_debayer[r][c].r)) / ((double) 65535));
                pixel_error += ENDORSE(((double) abs(debayer[r][c].g - gold_debayer[r][c].g)) / ((double) 65535));
                pixel_error += ENDORSE(((double) abs(debayer[r][c].b - gold_debayer[r][c].b)) / ((double) 65535));

                err += (pixel_error / ((double) 3))
                / ((double) ((WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD) * (WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD)));
            }
        }

        FILE *fp = fopen("err.txt", "wb");
        assert(fp != NULL);
        fprintf(fp, "%.16f\n", err);
        fclose(fp);
    }
#endif

#ifdef WRITE_OUTPUT_TO_DISK
    printf("\nWriting output to %s/%s.\n", output_directory, output_filename);
    {
        const u16 output_channels = 3;
        write_image_file(
            (char *) debayer,
            output_filename,
            output_directory,
            WAMI_DEBAYER_IMG_NUM_COLS - 2*PAD,
            WAMI_DEBAYER_IMG_NUM_ROWS - 2*PAD,
            output_channels);
    }
#endif

    FREE_AND_NULL(bayer);
    FREE_AND_NULL(debayer);
#ifdef ENABLE_CORRECTNESS_CHECKING
    FREE_AND_NULL(gold_debayer);
#endif

    return 0;
}
