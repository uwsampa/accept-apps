/*
 * distance.c
 *
 * Created on: Sep 9, 2013
 *          Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */


#include "distance.h"
#include <math.h>

int count = 0;
#define MAX_COUNT 1200000

void euclideanDistance(APPROX float* dist, RgbPixel* p, Centroid* c1) {
    APPROX float r;

    r = 0;

    r += (p->r - c1->r) * (p->r - c1->r);
    r += (p->g - c1->g) * (p->g - c1->g);
    r += (p->b - c1->b) * (p->b - c1->b);

    r = sqrtf(r);

    *dist = r;
}

int pickCluster(RgbPixel* p, Centroid* c1) {
    APPROX float d1;

    euclideanDistance(&d1, p, c1);

    if (p->distance <= ENDORSE(d1))
        return 0;

    return 1;
}

void assignCluster(RgbPixel* p, Clusters* clusters) {
    int i = 0;

    APPROX float d;
    euclideanDistance(&d, p, &clusters->centroids[i]);
    p->distance = ENDORSE(d);

    for(i = 1; i < clusters->k; ++i) {
        euclideanDistance(&d, p, &clusters->centroids[i]);
        if (ENDORSE(d) < p->distance) {
            p->cluster = i;
            p->distance = ENDORSE(d);
        }
    }
}

