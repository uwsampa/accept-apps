/*
 * kinematics.hpp
 *
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

 #ifndef __KINEMATICS_H__
 #define __KINEMATICS_H__

 //EnerC
 #include <enerc.h>

void forwardk2j(float theta1, float theta2, float* x, float* y);
void inversek2j(APPROX float* x, APPROX float* y, APPROX float* theta1, APPROX float* theta2);
 #endif