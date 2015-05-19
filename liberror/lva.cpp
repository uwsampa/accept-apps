#include "lva.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

#include <math.h>
#include <stdio.h>

const uint64 LVA::max_rand = -1;
LVA::lva_entry LVA::approximator[512];
LVA::dbl_uint64 LVA::GHB[4];
int LVA::GHB_head = 0;
float LVA::threshold = 0.1;
float LVA::pHitRate = 0.9;
bool LVA::init_done = false;

namespace {

  inline uint64 getRandom() {
    static uint64 x = 12345;
    x ^= (x >> 21);
    x ^= (x << 35);
    x ^= (x >> 4);
    return x;
  }
}

bool LVA::isCacheHit(uint64 addr) {
    const double rand_number = static_cast<double>(getRandom()) /
        static_cast<double>(max_rand);

    return (rand_number <= pHitRate);
}

uint64 LVA::getHash(uint64 pc) {
    return pc ^ GHB[0].b ^ GHB[1].b ^ GHB[2].b ^ GHB[3].b;
}


uint64 LVA::lvaLoad(uint64 ld_address, uint64 ret, const char* type, uint64 pc) {
    if(init_done == false)
	init();

    LVA::dbl_uint64 precise;
    precise.b = ret;
    printf("pc: %lld, ret: %llx, type: %s\n", pc, ret, type);
    printf("float: %f, double: %f\n", precise.f, precise.d);

    if (isCacheHit(ld_address)) {
	return ret;
    }

    dbl_uint64 retval;
    retval.b = ret;

    // miss in the cache

    // 
    uint64 idx = getHash(pc) & 0x1FF;
    if(approximator[idx].tag != getHash(pc)) {
	approximator[idx].confidence = -8;
	approximator[idx].degree = 0;
	approximator[idx].tag = getHash(pc);
    }
    
    if(approximator[idx].confidence >= 0) {
	//enough confidence to use predictor
    
	LVA::dbl_uint64 r;
	double p = (approximator[idx].LHB[0] +
	            approximator[idx].LHB[1] +
	            approximator[idx].LHB[2] +
	            approximator[idx].LHB[3]) / 4.0;
	
	if(strcmp(type, "Float") == 0)
	    retval.f = p;
	else
	    retval.d = p;
    }

    // update degree
    approximator[idx].degree--;
    if(approximator[idx].degree > 0)
	return retval.b;

    // train predictor
    approximator[idx].LHB_head = (approximator[idx].LHB_head + 1) & 0x3;
    if(strcmp(type, "Float") == 0)
        approximator[idx].LHB[approximator[idx].LHB_head] = precise.f;
    else
        approximator[idx].LHB[approximator[idx].LHB_head] = precise.d;

    // update GHB
    GHB_head = (GHB_head + 1) & 0x3;
    if(strcmp(type, "Float") == 0)
	GHB[GHB_head].f = precise.f;
    else
	GHB[GHB_head].f = precise.d;

    approximator[idx].degree = 1; 

    // compute confidence
    double error;
    if(strcmp(type, "Float") == 0)
	error = fabsf(fabsf(precise.f - retval.f)/precise.f);
    else
	error = fabs(fabs(precise.d - retval.d)/precise.d);
    if(error < 0.05) {
	approximator[idx].confidence++;
	if(approximator[idx].confidence == 8)
	    approximator[idx].confidence = 7;
    } else {
	approximator[idx].confidence--;
	if(approximator[idx].confidence == -9)
	    approximator[idx].confidence = -8;
    }

    return retval.b;
}

void LVA::init() {
    for(int i = 0; i < 512; i++) {
	approximator[i].degree = 0;
	approximator[i].confidence = -8;
	approximator[i].LHB_head = 0;
	approximator[i].tag = 0;
    }
    init_done = true;
}