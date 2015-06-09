/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/wami/kernels/change-detection/wami_kernel3_driver.c $
 * $Id: wami_kernel3_driver.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <enerc.h>

#include "wami_params.h"
#include "wami_utils.h"
#include "wami_gmm.h"
#include "wami_morpho.h"

#include "timer.h"

#if INPUT_SIZE == INPUT_SIZE_SMALL
    static const char *input_filename = "small_kernel3_input.bin";
    static const char *golden_output_filename = "small_golden_kernel3_output.bin";
    static const char *output_filename = "small_kernel3_output.bin";
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    static const char *input_filename = "medium_kernel3_input.bin";
    static const char *golden_output_filename = "medium_golden_kernel3_output.bin";
    static const char *output_filename = "medium_kernel3_output.bin";
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    static const char *input_filename = "large_kernel3_input.bin";
    static const char *golden_output_filename = "large_golden_kernel3_output.bin";
    static const char *output_filename = "large_kernel3_output.bin";
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

#define ENABLE_CORRECTNESS_CHECKING
#define WRITE_OUTPUT_TO_DISK

static void read_gmm_input_data(
    float mu[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    float sigma[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    float weights[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    u16 frames[WAMI_GMM_NUM_FRAMES][WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS],
    const char *filename,
    const char *directory);

int main(int argc, char **argv)
{
    float (*mu)[WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS] = NULL;
    float (*sigma)[WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS] = NULL;
    float (*weights)[WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS] = NULL;
    u8 (*foreground)[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS] = NULL;
#ifdef ENABLE_CORRECTNESS_CHECKING
    u8 (*golden_foreground)[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS] = NULL;
    u8 (*golden_eroded)[WAMI_GMM_IMG_NUM_COLS] = NULL;
    u8 (*eroded)[WAMI_GMM_IMG_NUM_COLS] = NULL;
#endif
    u8 (*morph)[WAMI_GMM_IMG_NUM_COLS] = NULL;
    u16 (*frames)[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS] = NULL;
    int i;

    char *input_directory = NULL;

    const size_t num_pixels = WAMI_GMM_IMG_NUM_ROWS * WAMI_GMM_IMG_NUM_COLS;

    if (argc != 2)
    {
        fprintf(stderr, "%s <directory-containing-input-files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_directory = argv[1];

    mu = XMALLOC(sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    sigma = XMALLOC(sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    weights = XMALLOC(sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    foreground = XMALLOC(sizeof(u8) * num_pixels * WAMI_GMM_NUM_FRAMES);
#ifdef ENABLE_CORRECTNESS_CHECKING
    golden_foreground = XMALLOC(sizeof(u8) * num_pixels * WAMI_GMM_NUM_FRAMES);
    eroded = XMALLOC(sizeof(u8) * num_pixels);
    golden_eroded = XMALLOC(sizeof(u8) * num_pixels);
#endif
    morph = XMALLOC(sizeof(u8) * num_pixels);
    frames = XMALLOC(sizeof(u16) * num_pixels * WAMI_GMM_NUM_FRAMES);

    memset(mu, 0, sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    memset(sigma, 0, sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    memset(weights, 0, sizeof(float) * num_pixels * WAMI_GMM_NUM_MODELS);
    memset(foreground, 0, sizeof(u8) * num_pixels * WAMI_GMM_NUM_FRAMES);
    memset(morph, 0, sizeof(u8) * num_pixels);
    memset(frames, 0, sizeof(u16) * num_pixels * WAMI_GMM_NUM_FRAMES);

    read_gmm_input_data(
        mu, sigma, weights, frames, input_filename, input_directory);

#ifdef ENABLE_CORRECTNESS_CHECKING
    read_data_file(
        (char *) golden_foreground,
        golden_output_filename,
        input_directory,
        sizeof(u8) * num_pixels * WAMI_GMM_NUM_FRAMES);
#endif

    printf("WAMI kernel 2 parameters:\n\n");
    printf("Image width = %d pixels\n", WAMI_DEBAYER_IMG_NUM_COLS);
    printf("Image height = %d pixels\n", WAMI_DEBAYER_IMG_NUM_ROWS);
    printf("Number of input frames = %d\n", WAMI_GMM_NUM_FRAMES);

    printf("\nStarting WAMI kernel 3 (Gaussian Mixture Model / Change Detection).\n\n");
    tic();
    accept_roi_begin();
    for (i = 0; i < WAMI_GMM_NUM_FRAMES; ++i)
    {
        wami_gmm(
            (u8 (*)[WAMI_GMM_IMG_NUM_COLS]) &foreground[i][0][0],
            mu,
            sigma,
            weights,
            (u16 (*)[WAMI_GMM_IMG_NUM_COLS]) &frames[i][0][0]);
    }
    accept_roi_end();
    PRINT_STAT_DOUBLE("CPU time using func toc - ", toc());
    printf ("\n");
#ifdef ENABLE_CORRECTNESS_CHECKING
    {
      int j, k, validation_warning = 0;
      double err;
        for (i = 0; i < WAMI_GMM_NUM_FRAMES; ++i)
        {
            int num_misclassified = 0, num_foreground = 0;
            double misclassification_rate = 0;

            wami_morpho_erode(
                eroded, (u8 (*)[WAMI_GMM_IMG_NUM_COLS]) &foreground[i][0][0]);
            wami_morpho_erode(
                golden_eroded, (u8 (*)[WAMI_GMM_IMG_NUM_COLS]) &golden_foreground[i][0][0]);

            printf("\nValidating frame %d output...\n", i);

            for (j = 0; j < WAMI_GMM_IMG_NUM_ROWS; ++j)
            {
                for (k = 0; k < WAMI_GMM_IMG_NUM_COLS; ++k)
                {
                    if (eroded[j][k] != golden_eroded[j][k])
                    {
                        ++num_misclassified;
                    }
                    if (golden_eroded[j][k] != 0)
                    {
                        ++num_foreground;
                    }
                }
            }
            misclassification_rate = (100.0*num_misclassified)/num_foreground;
	    err += (((double) num_misclassified) / ((double) num_foreground)) / ((double) WAMI_GMM_NUM_FRAMES);
            printf("\tMisclassified pixels: %d\n", num_misclassified);
            printf("\tGolden foreground pixels (after erosion): %d\n", num_foreground);
            printf("\tMisclassification rate relative to foreground: %f%%\n",
                misclassification_rate);
            if (misclassification_rate > 0.1)
            {
                validation_warning = 1;
            }
        }
	
	FILE *fp = fopen("err.txt", "wb");
	assert(fp != NULL);
	fprintf(fp, "%.16f\n", err);
	fclose(fp);

        if (validation_warning)
        {
            printf("\nValidation warning: Misclassification rate appears high; check images.\n\n");
        }
        else
        {
            printf("\nValidation checks passed.\n\n");
        }
    }
#endif
    
#ifdef WRITE_OUTPUT_TO_DISK
    printf("Writing output to %s.\n", output_filename);
    {
        FILE *fp = fopen(output_filename, "wb");
        assert(fp != NULL);
        assert(fwrite(foreground, sizeof(u8), num_pixels * WAMI_GMM_NUM_FRAMES, fp) ==
            num_pixels * WAMI_GMM_NUM_FRAMES);
        fclose(fp);
    }
#endif

    FREE_AND_NULL(mu);
    FREE_AND_NULL(sigma);
    FREE_AND_NULL(weights);
    FREE_AND_NULL(foreground);
#ifdef ENABLE_CORRECTNESS_CHECKING
    FREE_AND_NULL(golden_foreground);
    FREE_AND_NULL(eroded);
    FREE_AND_NULL(golden_eroded);
#endif
    FREE_AND_NULL(morph);
    FREE_AND_NULL(frames);

    return 0;
}

static void read_gmm_input_data(
    float mu[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    float sigma[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    float weights[WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS][WAMI_GMM_NUM_MODELS],
    u16 frames[WAMI_GMM_NUM_FRAMES][WAMI_GMM_IMG_NUM_ROWS][WAMI_GMM_IMG_NUM_COLS],
    const char *filename,
    const char *directory)
{
    char dir_and_filename[MAX_DIR_AND_FILENAME_LEN];
    FILE *fp = NULL;
    const size_t num_model_param_bytes = WAMI_GMM_IMG_NUM_ROWS * WAMI_GMM_IMG_NUM_COLS *
        WAMI_GMM_NUM_MODELS * sizeof(float);
    const size_t num_image_bytes = WAMI_GMM_IMG_NUM_ROWS * WAMI_GMM_IMG_NUM_COLS *
        sizeof(u16);
    size_t nread = 0;
    int success = 0, i;
    u16 width, height, channels, depth;

    assert(filename != NULL);
    assert(directory != NULL);

    concat_dir_and_filename(
        dir_and_filename,
        directory,
        filename);

    fp = fopen(dir_and_filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open input file %s for reading.\n",
            dir_and_filename);
        exit(EXIT_FAILURE);
    }

    success = (fread(mu, 1, num_model_param_bytes, fp) == num_model_param_bytes);
    success &= (fread(sigma, 1, num_model_param_bytes, fp) == num_model_param_bytes);
    success &= (fread(weights, 1, num_model_param_bytes, fp) == num_model_param_bytes);
    if (! success)
    {
        fprintf(stderr, "Error: Unable to read model parameters from %s.\n",
            dir_and_filename);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < WAMI_GMM_NUM_FRAMES; ++i)
    {
        success = (fread(&width, sizeof(u16), 1, fp) == 1);
        success &= (fread(&height, sizeof(u16), 1, fp) == 1);
        success &= (fread(&channels, sizeof(u16), 1, fp) == 1);
        success &= (fread(&depth, sizeof(u16), 1, fp) == 1);
        if (! success)
        {
            fprintf(stderr, "Error: Unable to read image %d header from %s.\n",
                i, dir_and_filename);
            exit(EXIT_FAILURE);
        }

        if (width != WAMI_GMM_IMG_NUM_COLS || height != WAMI_GMM_IMG_NUM_ROWS ||
            channels != 1 || depth != 2)
        {
            fprintf(stderr, "Error: Mismatch for image header %d in %s: "
                "[width,height,channels,depth] = [%u,%u,%u,%u].\n",
                i, dir_and_filename, width, height, channels, depth);
            exit(EXIT_FAILURE);
        }

        nread = fread(&frames[i][0][0], 1, num_image_bytes, fp);
        if (nread != num_image_bytes)
        {
            fprintf(stderr, "Error: Unable to read input image %d from %s.\n",
                i, dir_and_filename);
            exit(EXIT_FAILURE);
        } 
    }

    fclose(fp);
}
