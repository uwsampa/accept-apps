/*
 * convolution.h
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 *              Thierry Moreau <moreau@cs.washington.edu>
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <assert.h>
#include <enerc.h>

#include "rgb_image.h"

__attribute__((annotate("npu"))) float sobel(APPROX float i0, APPROX float i1, APPROX float i2, APPROX float i3, APPROX float i4, APPROX float i5, APPROX float i6, APPROX float i7, APPROX float i8);

#endif /* CONVOLUTION_H_ */
