/* -*-Mode: C;-*- */

/**BeginCopyright************************************************************
 *
 * $HeadURL: https://pastec.gtri.gatech.edu/svn/svn-dpc/INNC/projects/PERFECT-TAV-ES/suite/stap/kernels/lib/stap_utils.c $
 * $Id: stap_utils.c 8546 2014-04-02 21:36:22Z tallent $
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

#include "stap_utils.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <enerc.h>

// cconj and cmult are impure because of hidden memcpy calls,
// due to the definition of complex numbers as structs.
// I don't see a better way around this than just telling 
// ACCEPT that it's ok.

complex cconj(complex x)
{
    complex xconj = x; // ACCEPT_PERMIT
    xconj.im *= -1.0f;
    return xconj; // ACCEPT_PERMIT
}

complex cmult(complex lhs, complex rhs)
{
    complex prod;
    prod.re = lhs.re * rhs.re - lhs.im * rhs.im;
    prod.im = lhs.re * rhs.im + lhs.im * rhs.re;
    return prod; // ACCEPT_PERMIT
}

double calculate_snr(
    const complex *reference,
    const complex *test,
    size_t num_elements)
{
    double num = 0.0, den = 0.0;
    size_t i;

    for (i = 0; i < num_elements; ++i)
    {
        den += (ENDORSE(reference[i].re) - ENDORSE(test[i].re)) *
               (ENDORSE(reference[i].re) - ENDORSE(test[i].re));
        den += (ENDORSE(reference[i].im) - ENDORSE(test[i].im)) *
               (ENDORSE(reference[i].im) - ENDORSE(test[i].im));
        num += ENDORSE(reference[i].re) * ENDORSE(reference[i].re) +
               ENDORSE(reference[i].im) * ENDORSE(reference[i].im);
    }

    if (den == 0)
    {
        /* 
         * The test and reference sets are identical. Just
         * return a large number (in dB) rather than +infinity.
         */
        return 140.0;
    }
    else
    {
        return 10.0*log10(num/den);
    }
}

void *xmalloc(size_t size, const char *file, int line)
{
    void *x = malloc(size);
    if (x == NULL)
    {
        fprintf(stderr, "Error: memory allocation of size %lu at %s:%d.\n",
            size, file, line);
        exit(EXIT_FAILURE);
    }
    return x;
}

void extract_snapshot(
    complex snapshot[TDOF * N_CHAN],
    complex (* const datacube)[N_DOP][N_RANGE],
    int dop_index,
    int range_cell)
{
    int dof, chan;
    for (chan = 0; chan < N_CHAN; ++chan)
    {
        for (dof = 0; dof < TDOF; ++dof)
        {
            int dop = dop_index - (TDOF-1)/2 + dof;
            if (dop < 0) { dop += N_DOP; }
            if (dop >= N_DOP) { dop -= N_DOP; }

            snapshot[chan*TDOF+dof] = datacube[chan][dop][range_cell]; // ACCEPT_PERMIT
        }
    }
}

void read_complex_data_file(
    complex *data,
    const char *filename,
    const char *directory,
    size_t num_elements)
{
    size_t nread = 0;
    FILE *fp = NULL;
    char dir_and_filename[1024];

    assert(data != NULL);
    assert(filename != NULL);
    assert(directory != NULL);

    /* C89 lacks snprintf */
    if (strlen(directory) + strlen(filename) + 2 > sizeof(dir_and_filename))
    {
        fprintf(stderr, "Error: input directory (%s) too long.\n",
            directory);
        exit(EXIT_FAILURE);
    }
    dir_and_filename[0] = '\0';
    strncpy(dir_and_filename, directory, sizeof(dir_and_filename)-1);
    dir_and_filename[sizeof(dir_and_filename)-1] = '\0';
    strncat(dir_and_filename, "/",
        sizeof(dir_and_filename) - strlen(dir_and_filename) - 1);
    strncat(dir_and_filename, filename,
        sizeof(dir_and_filename) - strlen(dir_and_filename) - 1);

    printf("Reading %s...\n", dir_and_filename);

    fp = fopen(dir_and_filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open input file %s for reading.\n",
            dir_and_filename);
        exit(EXIT_FAILURE);
    }

    nread = fread(data, sizeof(complex), num_elements, fp);
    if (nread != num_elements)
    {
        fprintf(stderr, "Error: read failure on %s. "
            "Expected %lu elements, but only read %lu.\n",
            dir_and_filename, num_elements, nread);
        fclose(fp);
        exit(EXIT_FAILURE);
    }        

    fclose(fp);
}
