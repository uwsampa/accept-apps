/*
 * prototype.h
 *
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */

#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <stdlib.h>
#include "rgbimage.h"


UINT16 dspDivision(UINT32, UINT32);

void initQuantizationTables(UINT32);

UINT8* writeMarkers(UINT8 *, UINT32, UINT32, UINT32);
UINT8* closeBitstream(UINT8 *);

//UINT8* encodeImage(RgbImage*, UINT8 *, UINT32, UINT32);
UINT8* encodeImage(RgbImage*, UINT8 *, UINT32, UINT32);
//UINT8* encodeMcu(UINT32, UINT8*);
UINT8* encodeMcu(UINT32, UINT8*);

void levelShift(APPROX INT16 *);
void dct(APPROX INT16 *);
void quantization(APPROX INT16 *, APPROX UINT16 *, APPROX UINT16*);
UINT8* huffman(UINT16, UINT8 *);

#endif
