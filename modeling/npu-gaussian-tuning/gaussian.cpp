#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <limits>

#define TARGETERROR 0.05

typedef unsigned long long uint64;
typedef long long int64;


double generateGaussianNoise(double mu, double sigma)
{
  const double epsilon = std::numeric_limits<double>::min();
  const double two_pi = 2.0*3.14159265358979323846;
 
  static double z0, z1;
  static bool generate;
  generate = !generate;
 
  if (!generate)
     return z1 * sigma + mu;
 
  double u1, u2;
  do
   {
     u1 = rand() * (1.0 / RAND_MAX);
     u2 = rand() * (1.0 / RAND_MAX);
   }
  while ( u1 <= epsilon );
 
  z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
  z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
  return z0 * sigma + mu;
}

int main (int argc, char* argv[]){

  if (argc != 2) {
    printf("Pass in target MSE and value range\n");
    printf("usage: ./gaussian <NN MSE>\n");
    return -1;
  }

  float target = atof(argv[1]);
  // float valueRange = atof(argv[2]);
  printf("Target MSE: %f\n", target);

  float stddev = 10;
  double sum;
  bool errorMet = 0;

  while (!errorMet) {

    printf("Trying stddev of %f %%\n", stddev);

    sum = 0;
    for (int i = 0; i < 1000000; i++) {
      double sample = generateGaussianNoise(0, stddev)/100;
      sum += (sample*sample);
    }
    sum /= 1000000;
    printf("MSE = %f\n", sum);

    if (std::abs(sum - target)/sum<TARGETERROR) {
      errorMet = 1;
    } else if (sum > target) {
      stddev /= 1.01;
    } else {
      stddev *= 1.01;
    }

  }
}
