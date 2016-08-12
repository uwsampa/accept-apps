// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
//
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition, Prentice
// Hall, John C. Hull,

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//EnerC
#include <enerc.h>


//Precision to use for calculations
#define fptype float


int NUM_RUNS = 1;

typedef struct OptionData_ {
        APPROX fptype s;          // spot price
        APPROX fptype strike;     // strike price
        APPROX fptype r;          // risk-free interest rate
        APPROX fptype divq;       // dividend rate
        APPROX fptype v;          // volatility
        APPROX fptype t;          // time to maturity or option expiration in years
                                  //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)
        char OptionType;          // Option type.  "P"=PUT, "C"=CALL
        APPROX fptype divs;       // dividend vals (not used in this test)
        APPROX fptype DGrefval;   // DerivaGem Reference Value
} OptionData;

APPROX OptionData *data;
APPROX fptype *prices;
int numOptions;

int    * otype;
APPROX fptype * sptprice;
APPROX fptype * strike;
APPROX fptype * rate;
APPROX fptype * volatility;
APPROX fptype * otime;
int numError = 0;


void ACCEPTRegion(unsigned char* image, int image_size) {
}

////////////////////////////////////////////////////////////////////////////////
// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
static fptype inv_sqrt_2xPI = 0.39894228040143270286;

fptype CNDF ( fptype InputX )
{
    int sign;

    fptype OutputX;
    fptype xInput;
    fptype xNPrimeofX;
    fptype expValues;
    fptype xK2;
    fptype xK2_2, xK2_3;
    fptype xK2_4, xK2_5;
    fptype xLocal, xLocal_1;
    fptype xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (InputX < 0.0) {
        InputX = -InputX;
        sign = 1;
    } else
        sign = 0;

    xInput = InputX;

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = exp(-0.5f * InputX * InputX);
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

    xK2 = 0.2316419 * xInput;
    xK2 = 1.0 + xK2;
    xK2 = 1.0 / xK2;
    xK2_2 = xK2 * xK2;
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * 0.319381530;
    xLocal_2 = xK2_2 * -0.356563782;
    xLocal_3 = xK2_3 * 1.781477937;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_4 * -1.821255978;
    xLocal_2 = xLocal_2 + xLocal_3;
    xLocal_3 = xK2_5 * 1.330274429;
    xLocal_2 = xLocal_2 + xLocal_3;

    xLocal_1 = xLocal_2 + xLocal_1;
    xLocal   = xLocal_1 * xNPrimeofX;
    xLocal   = 1.0 - xLocal;

    OutputX  = xLocal;

    if (sign) {
        OutputX = 1.0 - OutputX;
    }

    return OutputX;
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// APPROX fptype BlkSchlsEqEuroNoDiv( APPROX fptype sptprice,
//                             APPROX fptype strike,
//                             APPROX fptype rate,
//                             APPROX fptype volatility,
//                             APPROX fptype time, int otype)
APPROX fptype BlkSchlsEqEuroNoDiv( APPROX OptionData* option,
                            int otype,
                            APPROX fptype* price)
{

    APPROX fptype sptprice = option->s;
    APPROX fptype strike = option->strike;
    APPROX fptype rate = option->r;
    APPROX fptype volatility = option->v;
    APPROX fptype time = option->t;

    // Define the constants
    static APPROX fptype ONE = 1.0;
    static APPROX fptype C0 = 0.5;

    APPROX fptype OptionPrice;

    // local private working variables for the calculation
    APPROX fptype xStockPrice;
    APPROX fptype xStrikePrice;
    APPROX fptype xRiskFreeRate;
    APPROX fptype xVolatility;
    APPROX fptype xTime;
    APPROX fptype xSqrtTime;

    APPROX fptype logValues;
    APPROX fptype xLogTerm;
    APPROX fptype xD1;
    APPROX fptype xD2;
    APPROX fptype xPowerTerm;
    APPROX fptype xDen;
    APPROX fptype d1;
    APPROX fptype d2;
    APPROX fptype FutureValueX;
    APPROX fptype NofXd1;
    APPROX fptype NofXd2;
    APPROX fptype NegNofXd1;
    APPROX fptype NegNofXd2;

    xStockPrice = sptprice; xStrikePrice = strike; xRiskFreeRate = rate; xVolatility = volatility;
    xTime = time;
    xSqrtTime = sqrtf(xTime);
    logValues = log(sptprice/strike); xLogTerm = logValues;

    xPowerTerm = xVolatility * xVolatility;
    xPowerTerm = xPowerTerm * C0;

    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 -  xDen;

    d1 = xD1;
    d2 = xD2;

    NofXd1 = CNDF( d1 );
    NofXd2 = CNDF( d2 );

    FutureValueX = strike * exp(-(rate)*(time));
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
    } else {
        NegNofXd1 = (ONE - NofXd1);
        NegNofXd2 = (ONE - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    *price = OptionPrice;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


int main (int argc, char **argv)
{
    FILE *file;
    int i, j;
    int rv;
    fflush(NULL);

    char *inputFile = argv[1];
    char *outputFile = argv[2];

    //Read input data from file
    file = fopen(inputFile, "r");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", inputFile);
      exit(1);
    }
    rv = fscanf(file, "%i", &numOptions);
    if(rv != 1) {
      printf("ERROR: Unable to read from file `%s'.\n", inputFile);
      fclose(file);
      exit(1);
    }


    // alloc spaces for the option data
    data = (OptionData*)malloc(numOptions*sizeof(OptionData));
    prices = (fptype*)malloc(numOptions*sizeof(fptype));
    for ( i = 0; i < numOptions; ++ i )
    {
        rv = fscanf(file, "%f %f %f %f %f %f %c %f %f", &data[i].s, &data[i].strike, &data[i].r, &data[i].divq, &data[i].v, &data[i].t, &data[i].OptionType, &data[i].divs, &data[i].DGrefval);
        if(rv != 9) {
          printf("ERROR: Unable to read from file `%s'.\n", inputFile);
          fclose(file);
          exit(1);
        }
    }
    rv = fclose(file);
    if(rv != 0) {
      printf("ERROR: Unable to close file `%s'.\n", inputFile);
      exit(1);
    }

    printf("Size of data: %d\n", numOptions * (sizeof(OptionData) + sizeof(int)));

    for (j=0; j<NUM_RUNS; j++) {

        for (i=0; i<numOptions; i++) {
            /* Calling main function to calculate option value based on
             * Black & Sholes's equation.
             */
            BlkSchlsEqEuroNoDiv(
                &data[i],
                (data[i].OptionType == 'P') ? 1 : 0,
                &prices[i]);

        }
    }

    //Write prices to output file
    file = fopen(outputFile, "w");
    if(file == NULL) {
      printf("ERROR: Unable to open file `%s'.\n", outputFile);
      exit(1);
    }
    rv = fprintf(file, "%i\n", numOptions);
    if(rv < 0) {
      printf("ERROR: Unable to write to file `%s'.\n", outputFile);
      fclose(file);
      exit(1);
    }
    for(i=0; i<numOptions; i++) {
      rv = fprintf(file, "%.18f\n", prices[i]);
      if(rv < 0) {
        printf("ERROR: Unable to write to file `%s'.\n", outputFile);
        fclose(file);
        exit(1);
      }
    }
    rv = fclose(file);
    if(rv != 0) {
      printf("ERROR: Unable to close file `%s'.\n", outputFile);
      exit(1);
    }
    free(data);
    free(prices);

    return 0;
}

