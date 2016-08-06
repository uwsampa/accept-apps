/*
 * kmeans.c
 *
 * Created on: Sep 9, 2013
 *          Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#include <stdio.h>
#include "rgbimage.h"
#include "segmentation.h"

int main (int argc, const char* argv[]) {

    RgbImage srcImage;
    Clusters clusters;

    initRgbImage(&srcImage);

    char *inputFile = argv[1];
    char *outputFile = argv[2];

    loadRgbImage(inputFile, &srcImage, 256);

    initClusters(&clusters, 6, 1);
    segmentImage(&srcImage, &clusters, 1);

    saveRgbImage(&srcImage, outputFile, 255);


    freeRgbImage(&srcImage);
    freeClusters(&clusters);
    return 0;
}

