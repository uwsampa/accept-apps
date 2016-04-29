#include <enerc.h>

#ifndef RGB_IMAGE_H_
#define RGB_IMAGE_H_

typedef unsigned char uchar;

typedef struct {
   APPROX uchar r;
   APPROX uchar g;
   APPROX uchar b;
   APPROX uchar lum;
} RgbPixel;


typedef struct {
   int w;
   int h;
   APPROX RgbPixel** pixels;
   char* meta;
} RgbImage;

void initRgbImage(APPROX RgbImage* image);
int loadRgbImage(const char* fileName, APPROX RgbImage* image);
int saveRgbImage(RgbImage* image, const char* fileName, float grayscale);
void freeRgbImage(RgbImage* image);


#endif /* RGB_IMAGE_H_ */
