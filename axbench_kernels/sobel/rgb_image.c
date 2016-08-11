/*
 * rgb_image.c
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#include "rgb_image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initRgbImage(RgbImage* image) {
    image->w = 0;
    image->h = 0;
    image->pixels = NULL;
    image->meta = NULL;
}

int readCell(FILE *fp, char* w) {
    int c;
    int i;

    w[0] = 0;
    for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) {
        if (c == ' ' || c == '\t') {
            if (w[0] != '\"')
                continue;
        }

        if (c == ',' || c == '\n') {
            if (w[0] != '\"')
                break;
            else if (c == '\"') {
                w[i] = c;
                i++;
                break;
            }
        }

        w[i] = c;
        i++;
    }
    w[i] = 0;

    return c;
}

int loadRgbImage(const char* fileName, RgbImage* image) {
    int c, r, g, b;
    int i;
    int j;
    int idx;
    char w[256];
    APPROX Pixel* pixels;
    FILE *fp;

    // printf("Loading %s ...\n", fileName);

    fp = fopen(fileName, "r");
    if (!fp) {
        printf("Warning: Oops! Cannot open %s!\n", fileName);
        return 0;
    }

    c = readCell(fp, w);
    image->w = atoi(w);
    c = readCell(fp, w);
    image->h = atoi(w);

    // printf("%d x %d\n", image->w, image->h);

    pixels = (Pixel*)malloc(image->h * image->w * sizeof(Pixel));

    if (pixels == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        fclose(fp);

        return 0;
    }

    idx = 0;
    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++, idx++) {
            c = readCell(fp, w);
            r = atoi(w);

            c = readCell(fp, w);
            g = atoi(w);

            c = readCell(fp, w);
            b = atoi(w);

            pixels[idx] = makePixGrayscale(r, g, b);
        }
    }
    image->pixels = pixels;

    c = readCell(fp, w);
    image->meta = (char*)malloc(strlen(w) * sizeof(char));
    if(image->meta == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        free(pixels);

        fclose(fp);

        return 0;

    }
    strcpy(image->meta, w);

    // printf("%s\n", image->meta);

    return 1;
}

int saveRgbImage(RgbImage* image, const char* fileName, APPROX float scale) {
    int i;
    int j;
    int idx;
    int pixel;
    FILE *fp;

    fp = fopen(fileName, "w");
    if (!fp) {
        printf("Warning: Oops! Cannot open %s!\n", fileName);
        return 0;
    }

    fprintf(fp, "%d,%d\n", image->w, image->h);

    idx = 0;
    for(i = 0; i < image->h; i++) {
        for(j = 0; j < (image->w - 1); j++, idx++) {
            pixel = ENDORSE(image->pixels[idx] * scale);
            if (pixel > 255) {
                pixel = 255;
            }
            fprintf(fp, "%d,%d,%d,", pixel, pixel, pixel);
        }
        idx++;
        pixel = ENDORSE(image->pixels[idx] * scale);
        if (pixel > 255) {
            pixel = 255;
        }
        fprintf(fp, "%d,%d,%d\n", pixel, pixel, pixel);
    }

    fprintf(fp, "%s", image->meta);

    fclose(fp);

    return 1;
}

void freeRgbImage(RgbImage* image) {
    int i;

    if (image->meta != NULL)
        free(image->meta);

    if (image->pixels == NULL)
        return;

    free(image->pixels);
}

APPROX float makePixGrayscale(int r, int g, int b) {
    APPROX float luminance;

    APPROX float rC = 0.30 / 256.0;
    APPROX float gC = 0.59 / 256.0;
    APPROX float bC = 0.11 / 256.0;

    luminance =
        rC * (float) r +
        gC * (float) g +
        bC * (float) b;

    return luminance;
}
