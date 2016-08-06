/*
 * rgbimage.h
 *
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#ifndef RGB_IMAGE_H_
#define RGB_IMAGE_H_

//EnerC
#include <enerc.h>

typedef struct {
   APPROX float r;
   APPROX float g;
   APPROX float b;
   int cluster;
   float distance;
} RgbPixel;

typedef struct {
   int w;
   int h;
   RgbPixel** pixels;
   char* meta;
} RgbImage;

void initRgbImage(RgbImage* image);
int loadRgbImage(const char* fileName, RgbImage* image, float scale);
int saveRgbImage(RgbImage* image, const char* fileName, float scale);
void freeRgbImage(RgbImage* image);

void makeGrayscale(RgbImage* rgbImage);

#endif /* RGB_IMAGE_H_ */
