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
    makeGrayscale(&srcImage);

    accept_roi_begin();


    for (y = 0; y < srcImage.h; y++) {

      for (x = 0; x < srcImage.w; x++) {
        if (x==0 || x==srcImage.w-1 || y==0 || y==srcImage.h-1) {
          s = 0;
        } else {
          i0 = srcImage.pixels[y-1][x-1].r;
          i1 = srcImage.pixels[y-1][x].r;
          i2 = srcImage.pixels[y-1][x+1].r;
          i3 = srcImage.pixels[y][x-1].r;
          i4 = srcImage.pixels[y][x].r;
          i5 = srcImage.pixels[y][x+1].r;
          i6 = srcImage.pixels[y+1][x-1].r;
          i7 = srcImage.pixels[y+1][x].r;
          i8 = srcImage.pixels[y+1][x+1].r;

          s = sobel(i0, i1, i2, i3, i4, i5, i6, i7, i8);
        }

        dstImage.pixels[y][x].r = ENDORSE(s);
        dstImage.pixels[y][x].g = ENDORSE(s);
        dstImage.pixels[y][x].b = ENDORSE(s);
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


