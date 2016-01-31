/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/stap/kernels/system-solve/stap_kernel2_driver.c $
 * $Id: stap_kernel2_driver.c 8546 2014-04-02 21:36:22Z tallent $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <enerc.h>

#include "stap_params.h"
#include "stap_utils.h"
#include "stap_system_solver.h"

#define WRITE_OUTPUT_TO_DISK

#if INPUT_SIZE == INPUT_SIZE_SMALL
    static const char *kernel1_output_filename = "small_kernel1_output.bin";
    static const char *kernel2_output_filename = "small_kernel2_output.bin";
    static const char *steering_vector_filename = "small_steering_vectors.bin";
#elif INPUT_SIZE == INPUT_SIZE_MEDIUM
    static const char *kernel1_output_filename = "medium_kernel1_output.bin";
    static const char *kernel2_output_filename = "medium_kernel2_output.bin";
    static const char *steering_vector_filename = "medium_steering_vectors.bin";
#elif INPUT_SIZE == INPUT_SIZE_LARGE
    static const char *kernel1_output_filename = "large_kernel1_output.bin";
    static const char *kernel2_output_filename = "large_kernel2_output.bin";
    static const char *steering_vector_filename = "large_steering_vectors.bin";
#else
    #error "Unhandled value for INPUT_SIZE"
#endif

#ifdef WRITE_OUTPUT_TO_DISK
    static const char* output_filename = "out.bin";
#endif

int main(int argc, char **argv)
{
    complex (*covariances)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF] = NULL;
    complex (*cholesky_factors)[N_BLOCKS][N_CHAN*TDOF][N_CHAN*TDOF] = NULL;
    complex (*adaptive_weights)[N_BLOCKS][N_STEERING][N_CHAN*TDOF] = NULL;
    complex (*steering_vectors)[N_CHAN*TDOF] = NULL;
    char *input_directory = NULL;

#ifdef ENABLE_CORRECTNESS_CHECKING
    complex (*gold_weights)[N_BLOCKS][N_STEERING][N_CHAN*TDOF] = NULL;
#endif

    const size_t num_covariance_elements = (TDOF*N_CHAN) * (TDOF*N_CHAN) *
        N_DOP * N_BLOCKS;
    const size_t num_adaptive_weight_elements = N_DOP * N_BLOCKS *
        N_STEERING * (N_CHAN*TDOF);
    const size_t num_steering_vector_elements = N_STEERING *
        (N_CHAN*TDOF);

    if (argc != 2)
    {
        fprintf(stderr, "%s <directory-containing-input-files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    input_directory = argv[1];

    covariances = XMALLOC(sizeof(complex) * num_covariance_elements);
    cholesky_factors = XMALLOC(sizeof(complex) * num_covariance_elements);
    adaptive_weights = XMALLOC(sizeof(complex) * num_adaptive_weight_elements);
    steering_vectors = XMALLOC(sizeof(complex) * num_steering_vector_elements);

#ifdef ENABLE_CORRECTNESS_CHECKING
    gold_weights = XMALLOC(sizeof(complex) * num_adaptive_weight_elements);
#endif

    printf("Loading input files from %s...\n", input_directory);

    read_complex_data_file(
        (complex *) covariances,
        kernel1_output_filename,
        input_directory,
        num_covariance_elements);

    read_complex_data_file(
        (complex *) steering_vectors,
        steering_vector_filename,
        input_directory,
        num_steering_vector_elements);

#ifdef ENABLE_CORRECTNESS_CHECKING
    read_complex_data_file(
        (complex *) gold_weights,
        kernel2_output_filename,
        input_directory,
        num_adaptive_weight_elements);
#endif

    memset(adaptive_weights, 0,
        sizeof(complex) * num_adaptive_weight_elements);

    /* Kernel 2: Weight Generation (Linear System Solver) */
    printf("Calling STAP Kernel 2 -- weight generation / linear system solver...\n");
    accept_roi_begin();
    stap_system_solver(
        adaptive_weights,
        covariances,
        steering_vectors,
        cholesky_factors);
    accept_roi_end();
#ifdef ENABLE_CORRECTNESS_CHECKING
    {
        double snr;
        printf("Computing correctness metrics...\n");
        snr = calculate_snr(
            (complex *) gold_weights,
            (complex *) adaptive_weights,
            num_adaptive_weight_elements);
        printf("\tSNR after STAP kernel 2 : %.2f dB\n", snr);
    FILE *fp = fopen("snr.txt", "wb");
    assert(fp != NULL);
    fprintf(fp, "%.2f\n", snr);
    fclose(fp);
    }
    FREE_AND_NULL(gold_weights);
#endif

#ifdef WRITE_OUTPUT_TO_DISK
    printf("\nWriting output to %s/%s.\n", input_directory, kernel2_output_filename);
    {
        write_complex_data_file(
            adaptive_weights,
            output_filename,
            input_directory,
            num_adaptive_weight_elements);
    }
#endif

    FREE_AND_NULL(covariances);
    FREE_AND_NULL(cholesky_factors);
    FREE_AND_NULL(adaptive_weights);
    FREE_AND_NULL(steering_vectors);

    return 0;
}
