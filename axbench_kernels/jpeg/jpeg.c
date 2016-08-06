/*
 * jpeg.c
 *
 * Created on: Sep 9, 2013
 *          Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatype.h"
#include "jpegconfig.h"
#include "prototype.h"

#include "rgbimage.h"


#define OUT_BUFFER_SIZE 262144 // in bytes


int main (int argc, const char* argv[]) {
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    UINT32 qualityFactor;
    UINT32 imageFormat;

    UINT8 *outputBuffer;
    FILE *fp;

    UINT8 *outputBufferPtr;

    qualityFactor = 1024;
    imageFormat = GRAY;


    RgbImage srcImage;
    initRgbImage(&srcImage);
    if (loadRgbImage(inputFile, &srcImage) == 0) {
        printf("Error! Oops: Cannot load the input image!\n");
        return -1;
    }

    makeGrayscale(&srcImage);

    outputBuffer = (UINT8 *) malloc(OUT_BUFFER_SIZE * sizeof(UINT8));


    outputBufferPtr = outputBuffer;
    outputBufferPtr = encodeImage(
        &srcImage, outputBufferPtr, qualityFactor, imageFormat
    );


    freeRgbImage(&srcImage);

    fp = fopen(outputFile, "wb");
    if (fp != NULL) {
        fwrite(outputBuffer, 1, srcImage.w*srcImage.h, fp);
        fclose(fp);
    }
    free(outputBuffer);
    return 0;
}

