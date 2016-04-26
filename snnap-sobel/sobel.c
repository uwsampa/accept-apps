#include "rgb_image.h"
#include "convolution.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <enerc.h>

int main (int argc, const char* argv[]) {

    // Sobel variables
    int i;
    int x, y;
    APPROX float s;
    APPROX RgbImage srcImage;
    APPROX RgbImage dstImage;
    APPROX float i0;
    APPROX float i1;
    APPROX float i2;
    APPROX float i3;
    APPROX float i4;
    APPROX float i5;
    APPROX float i6;
    APPROX float i7;
    APPROX float i8;

    // Init the rgb images
    initRgbImage(&srcImage);
    initRgbImage(&dstImage);
    loadRgbImage(argv[1], &srcImage);
    loadRgbImage(argv[1], &dstImage);
    // makeGrayscale(&srcImage);

    accept_roi_begin();


    for (y = 0; y < srcImage.h; y++) {

      for (x = 0; x < srcImage.w; x++) {
        if (x==0 || x==srcImage.w-1 || y==0 || y==srcImage.h-1) {
          dstImage.pixels[y][x].lum = 0;
        } else {
          i0 = srcImage.pixels[y-1][x-1].lum;
          i1 = srcImage.pixels[y-1][x].lum;
          i2 = srcImage.pixels[y-1][x+1].lum;
          i3 = srcImage.pixels[y][x-1].lum;
          i4 = srcImage.pixels[y][x].lum;
          i5 = srcImage.pixels[y][x+1].lum;
          i6 = srcImage.pixels[y+1][x-1].lum;
          i7 = srcImage.pixels[y+1][x].lum;
          i8 = srcImage.pixels[y+1][x+1].lum;

          sobel(&(dstImage.pixels[y][x].lum), i0, i1, i2, i3, i4, i5, i6, i7, i8);
        }
      }
    }

    // Gray Scale
    for (y = 0; y < srcImage.h; y++) {
      for (x = 0; x < srcImage.w; x++) {
      }
    }

    accept_roi_end();

    // Save the file now
    saveRgbImage(ENDORSE(&dstImage), "out.rgb", sqrt(256 * 256 + 256 * 256));

    ///////////////////////////////
    // 6 - Free memory
    ///////////////////////////////

    freeRgbImage(ENDORSE(&srcImage));
    freeRgbImage(ENDORSE(&dstImage));

    return 0;
}


