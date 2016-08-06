/*
 * kinematics.cpp
 *
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <math.h>
#include "kinematics.h"

APPROX static float l1 = 0.5 ;
APPROX static float l2 = 0.5 ;

// ACCEPT FIXME: constant propagation does not work with appox
APPROX static float CST_0 = -0.5; // - (l1 * l1) - (l2 * l2)
APPROX static float CST_1 = 2; // 1.0 / (2 * l1 * l2)

void forwardk2j(APPROX float theta1, APPROX float theta2, APPROX float* x, APPROX float* y) {
	*x = l1 * cos(theta1) + l2 * cos(theta1 + theta2) ;
	*y = l1 * sin(theta1) + l2 * sin(theta1 + theta2) ;
}

void inversek2j(APPROX float x, APPROX float y, APPROX float* theta1, APPROX float* theta2) {
    APPROX static float num;
    APPROX static float denom;
    APPROX static float t2;

    denom = x * x + y * y;
    t2 = acos((denom + CST_0)*2);
    num = y * (l1 + l2 * cos(t2)) - x * l2 * sin(t2);
    *theta1 = asin( num / denom );
    *theta2 = t2;
}