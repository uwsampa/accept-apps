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


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "xmalloc.h"

#ifdef _OPENMP
#define OMP(x) _Pragma(x)
#else
#define OMP(x)
#endif

/**
* @brief Self-checking wrapper to malloc()
*
* @param sz Number of bytes to allocate
*
* @return 
*/
void *
xmalloc(size_t sz)
{
  void * out;
  out = malloc(sz);
  if (!out) {
    perror("Failed xmalloc: ");
    abort();
  }

  return out;
}

/**
* @brief Self-checking wrapper to calloc()
*
* @param nelem Number of elements
* @param sz Size of each element
*
* @return 
*/
void *
xcalloc(size_t nelem, size_t sz)
{
  void * out;
  out = calloc(nelem, sz);
  if (!out) {
    perror("Failed xcalloc: ");
    abort();
  }
  return out;
}

/**
* @brief Self-checking wrapper to realloc()
*
* @param p Pointer to current allocation
* @param sz Number of bytes of new allocation
*
* @return 
*/
void *
xrealloc(void *p, size_t sz)
{
  void * out;
  out = realloc(p, sz);
  if (!out) {
    perror("Failed xrealloc: ");
    abort();
  }
  return out;
}

/**
* @brief Self-checking wrapper to mmap()
*
* @param addr Starting address for new mapping
* @param len Length of the mapping
* @param prot Memory protection of the mapping
* @param flags MAP_SHARED or MAP_PRIVATE, plus other flags
* @param fd File descriptor
* @param offset Offset
*
* @return 
*/
void *
xmmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
  void * out;
  out = mmap(addr, len, prot, flags, fd, offset);
  if (MAP_FAILED != out)
    return out;
  perror("mmap failed");
  abort();
  return NULL;
}

/**
* @brief Initialize all elements of an array
*
* @param s Array to initialize
* @param c Value to set
* @param num Number of elements in the array
*/
void xelemset(int64_t *s, int64_t c, int64_t num)
{
  int64_t v;
  assert(s != NULL);
  OMP("omp parallel for")
    for (v = 0; v < num; ++v) {
      s[v] = c;
    }
}

/**
* @brief Copy from one array to another
*
* @param dest Output array
* @param src Input array
* @param num Number of elements to copy
*/
void xelemcpy(int64_t *dest, int64_t *src, int64_t num)
{
  assert(dest != NULL && src != NULL);
#ifdef _OPENMP
  int64_t v;
  OMP("omp parallel for")
    for (v = 0; v < num; ++v)
    {
      dest[v] = src[v];
    }
#else
  memcpy(dest, src, num * sizeof(int64_t));
#endif
}


/**
 * @brief Write zeros to sz bytes starting at x.
 *
 * @param x Pointer to memory location
 * @param sz Memory size in bytes
 */
void xzero(void *x, const size_t sz)
{
  memset (x, 0, sz);
}
