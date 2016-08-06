/*
 * convolution.h
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "rgb_image.h"

void sobel(APPROX Pixel* src, APPROX Pixel* dst, int y, int x, int h, int w);

#endif /* CONVOLUTION_H_ */
