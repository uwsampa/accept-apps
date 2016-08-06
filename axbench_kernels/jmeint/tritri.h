/*
 * tritri.h
 *
 *  Created on: Apr 25, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#ifndef TRITRI_H_
#define TRITRI_H_

//EnerC
#include <enerc.h>

static APPROX float ZERO = 0.0f;

int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
                     float U0[3],float U1[3],float U2[3]);

void isect2(float VTX0[3],float VTX1[3],float VTX2[3],float VV0,float VV1,float VV2,
        float D0,float D1,float D2,float *isect0,float *isect1,float isectpoint0[3],float isectpoint1[3]);

int coplanar_tri_tri(APPROX float N[3],APPROX float V0[3],APPROX float V1[3],APPROX float V2[3],
                     APPROX float U0[3],APPROX float U1[3],APPROX float U2[3]);

#endif /* TRITRI_H_ */
