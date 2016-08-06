/*
 * sobel.c
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#include "rgb_image.h"
#include "convolution.h"
#include <stdio.h>
#include <math.h>

int main (int argc, const char* argv[]) {
    int x, y;
    float scale = sqrt(256 * 256 + 256 * 256);
    APPROX RgbImage srcImage;
    APPROX RgbImage dstImage;
    char *inputFile = argv[1];
    char *outputFile = argv[2];

    initRgbImage(ENDORSE(&srcImage));
    initRgbImage(ENDORSE(&dstImage));
    loadRgbImage(inputFile, ENDORSE(&srcImage));
    loadRgbImage(inputFile, ENDORSE(&dstImage));
    // makeGrayscale(ENDORSE(&srcImage));

    for (y = 1; y < srcImage.h - 1; y++) {
        for (x = 1; x < srcImage.w - 1; x++) {
            sobel(srcImage.pixels, dstImage.pixels, y, x, srcImage.h, srcImage.w);
        }
    }

    saveRgbImage(ENDORSE(&dstImage), outputFile, scale);

    return 0;
}

