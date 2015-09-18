/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/sar/kernels/pfa-interp2/sar_kernel2_driver.c $
 * $Id: sar_kernel2_driver.c 8546 2014-04-02 21:36:22Z tallent $
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
#include <math.h>
#include <assert.h>
#include <enerc.h>

#include "sar_params.h"
#include "sar_utils.h"
#include "sar_interp2.h"

#define ENABLE_CORRECTNESS_CHECKING

#if INPUT_SIZE == INPUT_SIZE_SMALL
    static const char *output_filename = "small_kernel2_output.bin";
    static const char *golden_output_filename = "small_golden_kernel2_output.bin";
    static const char *input_filename = "small_kernel2_input.bin";
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    static const char *output_filename = "medium_kernel2_output.bin";
    static const char *golden_output_filename = "medium_golden_kernel2_output.bin";
    static const char *input_filename = "medium_kernel2_input.bin";
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    static const char *output_filename = "large_kernel2_output.bin";
    static const char *golden_output_filename = "large_golden_kernel2_output.bin";
    static const char *input_filename = "large_kernel2_input.bin";
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

#define WRITE_OUTPUT_TO_DISK

static void read_kern2_data_file(
    const char *input_filename,
    const char *directory,
    complex (*data)[PFA_NOUT_RANGE],
    double *input_coords,
    double *output_coords,
    float *window);

int main(int argc, char **argv)
{
    complex (*data)[PFA_NOUT_RANGE] = NULL;
    complex (*resampled)[PFA_NOUT_RANGE] = NULL;
    double (*input_coords)[N_PULSES] = NULL;
    double *output_coords = NULL;
    float *window = NULL;
    const char *input_directory = NULL;
#ifdef ENABLE_CORRECTNESS_CHECKING
    complex (*gold_resampled)[PFA_NOUT_RANGE] = NULL;
#endif

    const size_t num_data_elements = N_PULSES * PFA_NOUT_RANGE;
    const size_t num_resampled_elements = PFA_NOUT_AZIMUTH * PFA_NOUT_RANGE;
    const size_t num_window_elements = T_PFA;

    if (argc != 2)
    {
        fprintf(stderr, "%s <directory-containing-input-files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_directory = argv[1];

    data = XMALLOC(sizeof(complex) * num_data_elements);
    resampled = XMALLOC(sizeof(complex) * num_resampled_elements);
    input_coords = XMALLOC(sizeof(double) * PFA_NOUT_RANGE * N_PULSES);
    output_coords = XMALLOC(sizeof(double) * PFA_NOUT_RANGE);
    window = XMALLOC(sizeof(float) * num_window_elements);
#ifdef ENABLE_CORRECTNESS_CHECKING
    gold_resampled = XMALLOC(sizeof(complex) * num_resampled_elements);
#endif

    read_kern2_data_file(
        input_filename,
        input_directory,
        data,
        (double *) input_coords,
        output_coords,
        window);

#ifdef ENABLE_CORRECTNESS_CHECKING
    read_data_file(
        (char *) gold_resampled,
        golden_output_filename,
        input_directory,
        sizeof(complex)*num_resampled_elements);
#endif

    printf("SAR kernel 2 parameters:\n\n");
    printf("Number of pulses: %d\n", N_PULSES);
    printf("Number of resampled range frequencies: %d\n", PFA_NOUT_RANGE);
    printf("Number of resampled azimuth frequencies: %d\n", PFA_NOUT_AZIMUTH);
    printf("Output coordinate spacing: %f\n",
        output_coords[1] - output_coords[0]);
    /*
     * The input coordinate spacing can vary: we only include this output as
     * a sanity check (i.e., to make sure the file was read correctly).
     */
    printf("Input coordinate spacing (range freq 0; sanity check only): %f\n",
        input_coords[0][1] - input_coords[0][0]);
    printf("Output dimensions: %d x %d\n",
        PFA_NOUT_AZIMUTH, PFA_NOUT_RANGE);

    printf("\nStarting kernel 2 (PFA interp2 / azimuth interpolation).\n");
    accept_roi_begin();
    /* SAR Kernel 2: Interpolation 2 (PFA azimuth interpolation) */
    sar_interp2(
        resampled,
        data,
        window,
        input_coords,
        output_coords);
    accept_roi_end();
    printf("Kernel complete.\n");

#ifdef WRITE_OUTPUT_TO_DISK
    printf("Saving output to %s.\n", output_filename);
    {
        FILE *fp = fopen(output_filename, "wb");
        assert(fp != NULL);
        assert(fwrite(resampled, sizeof(complex), num_resampled_elements, fp) ==
            num_resampled_elements);
        fclose(fp);
    }
#endif

#ifdef ENABLE_CORRECTNESS_CHECKING
    {
        double snr = calculate_snr(
            (complex *) gold_resampled,
            (complex *) resampled,
            num_resampled_elements);
        printf("\nImage correctness SNR = %.2f\n", snr);
    #ifdef AUTOTUNER
        FILE *fp = fopen("out.txt", "wb");
    #else
        FILE *fp = fopen("snr.txt", "wb");
    #endif //AUTOTUNER
    assert(fp != NULL);
    fprintf(fp, "%.2f\n", snr);
    fclose(fp);
    }
#endif

    FREE_AND_NULL(data);
    FREE_AND_NULL(resampled);
    FREE_AND_NULL(input_coords);
    FREE_AND_NULL(output_coords);
    FREE_AND_NULL(window);
#ifdef ENABLE_CORRECTNESS_CHECKING
    FREE_AND_NULL(gold_resampled);
#endif

    return 0;
}

void read_kern2_data_file(
    const char *input_filename,
    const char *input_directory,
    complex (*data)[PFA_NOUT_RANGE],
    double *input_coords,
    double *output_coords,
    float *window)
{
    FILE *fp = NULL;
    const size_t num_data_elements = PFA_NOUT_RANGE*N_PULSES;
    const size_t num_window_elements = T_PFA;
    char dir_and_filename[MAX_DIR_AND_FILENAME_LEN];
    size_t n;

    concat_dir_and_filename(
        dir_and_filename,
        input_directory,
        input_filename);

    fp = fopen(dir_and_filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open input filename %s.\n",
            input_filename);
        exit(EXIT_FAILURE);
    }

    if ((n = fread(data, sizeof(complex), num_data_elements, fp)) !=
        num_data_elements)
    {
        fprintf(stderr, "Error: Unable to read phase history data from %s "
            "(read %lu elements instead of %lu).\n",
            input_filename, n, num_data_elements);
        exit(EXIT_FAILURE);
    }

    if ((n = fread(input_coords, sizeof(double), N_PULSES * PFA_NOUT_RANGE, fp)) !=
        N_PULSES * PFA_NOUT_RANGE)
    {
        fprintf(stderr, "Error: Unable to read input coordinates from %s "
            "(read %lu elements instead of %d).\n",
            input_filename, n, N_PULSES * PFA_NOUT_RANGE);
        exit(EXIT_FAILURE);
    }

    if ((n = fread(output_coords, sizeof(double), PFA_NOUT_AZIMUTH, fp)) !=
        PFA_NOUT_AZIMUTH)
    {
        fprintf(stderr, "Error: Unable to read output coordinates from %s "
            "(read %lu elements instead of %d).\n",
            input_filename, n, PFA_NOUT_AZIMUTH);
        exit(EXIT_FAILURE);
    }

    if ((n = fread(window, sizeof(float), num_window_elements, fp)) !=
        num_window_elements)
    {
        fprintf(stderr, "Error: Unable to read window values from %s "
            "(read %lu elements instead of %lu;).\n",
            input_filename, n, num_window_elements);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
}
