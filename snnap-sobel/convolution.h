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

__attribute__((annotate("npu"))) void sobel(APPROX uchar *dst, APPROX uchar i0, APPROX uchar i1, APPROX uchar i2, APPROX uchar i3, APPROX uchar i4, APPROX uchar i5, APPROX uchar i6, APPROX uchar i7, APPROX uchar i);

#endif /* CONVOLUTION_H_ */
