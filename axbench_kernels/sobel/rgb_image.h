/*
 * rgb_image.h
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#ifndef RGB_IMAGE_H_
#define RGB_IMAGE_H_

//EnerC
#include <enerc.h>

// typedef struct {
//    APPROX float r;
//    APPROX float g;
//    APPROX float b;
// } RgbPixel;

typedef APPROX float Pixel;

typedef struct {
   int w;
   int h;
   APPROX Pixel* pixels;
   char* meta;
} RgbImage;

void initRgbImage(RgbImage* image);
int loadRgbImage(const char* fileName, RgbImage* image);
int saveRgbImage(RgbImage* image, const char* fileName, float grayscale);
void freeRgbImage(RgbImage* image);

// void makeGrayscale(RgbImage* rgbImage);
APPROX float makePixGrayscale(int r, int g, int b);

#endif /* RGB_IMAGE_H_ */
