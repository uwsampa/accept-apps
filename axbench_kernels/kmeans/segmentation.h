/*
 * segmentation.h
 *
 * Created on: Sep 9, 2013
 *          Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#ifndef SEGMENTATION_H_
#define SEGMENTATION_H_

#include "rgbimage.h"

typedef struct {
   APPROX float r;
   APPROX float g;
   APPROX float b;
   int n;
} Centroid;

typedef struct {
   int k;
   Centroid* centroids;
} Clusters;

int initClusters(Clusters* clusters, int k, float scale);
void segmentImage(RgbImage* image, Clusters* clusters, int n);
void freeClusters(Clusters* clusters);

#endif /* SEGMENTATION_H_ */
