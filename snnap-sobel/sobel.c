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
    APPROX RgbImage srcImage;
    APPROX RgbImage dstImage;
    APPROX uchar i0;
    APPROX uchar i1;
    APPROX uchar i2;
    APPROX uchar i3;
    APPROX uchar i4;
    APPROX uchar i5;
    APPROX uchar i6;
    APPROX uchar i7;
    APPROX uchar i8;

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

    accept_roi_end();

    // Save the file now
    saveRgbImage(ENDORSE(&dstImage), "out.rgb", 1);

    ///////////////////////////////
    // 6 - Free memory
    ///////////////////////////////

    freeRgbImage(ENDORSE(&srcImage));
    freeRgbImage(ENDORSE(&dstImage));

    return 0;
}


