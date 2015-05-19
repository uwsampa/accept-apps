#include "lva.hpp"

#include<cstring>
#include<ctime>
#include<iostream>
#include<cstdlib>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const uint64 LVA::max_rand = -1;
LVA::lva_entry LVA::approximator[512];
LVA::dbl_uint64 LVA::GHB[4];
int    LVA::GHB_head = 0;
bool   LVA::init_done = false;
uint64 LVA::stats_accesses = 0;
uint64 LVA::stats_cache_misses = 0;
uint64 LVA::stats_predictions = 0;

float  LVA::threshold = 0.05; // error threshold - determines if prediction was accurate
float  LVA::pHitRate = 0.9;   // cache hit rate
int    LVA::degree = 1;       // how often is prediction value used

#define fuzzy_mantissa_sft 10

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
    return pc ^
           (GHB[0].b >> fuzzy_mantissa_sft) ^
           (GHB[1].b >> fuzzy_mantissa_sft) ^
           (GHB[2].b >> fuzzy_mantissa_sft) ^
           (GHB[3].b >> fuzzy_mantissa_sft);
}


uint64 LVA::lvaLoad(uint64 ld_address, uint64 ret, const char* type, uint64 pc) {
    if(init_done == false)
	init();
    stats_accesses++;

    if (isCacheHit(ld_address)) {
	//printf("Cache hit\n");
	return ret;
    }
    stats_cache_misses++;

    LVA::dbl_uint64 precise;
    precise.b = ret;
    //printf("pc: %lld, ret: %llx, type: %s\n", pc, ret, type);
    //printf("float: %f, double: %f\n", precise.f, precise.d);

    dbl_uint64 retval;
    retval.b = ret;

    // miss in the cache

    uint64 tag = getHash(pc);
    uint64 idx = tag & 0x1FF;
    //printf("tag: %llx, idx: %lld\n", tag, idx);
    if(approximator[idx].tag != tag) {
	//printf("Tag miss\n");
	approximator[idx].confidence = -8;
	approximator[idx].degree = 0;
	approximator[idx].tag = tag;
    }

    if(approximator[idx].confidence >= 0) {
	//enough confidence to use predictor
	printf("Using LVA\n");
	stats_predictions++;
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

    approximator[idx].degree = degree;

    // compute confidence
    double error;
    if(strcmp(type, "Float") == 0)
	error = fabsf(fabsf(precise.f - retval.f)/precise.f);
    else
	error = fabs(fabs(precise.d - retval.d)/precise.d);
    if(error < threshold) {
	//printf("[%lld]Increasing confidence! Error: %f\n", idx, error);
	approximator[idx].confidence++;
	if(approximator[idx].confidence == 8)
	    approximator[idx].confidence = 7;
    } else {
	//printf("[%lld]Decreasing confidence! Error: %f\n", idx, error);
	approximator[idx].confidence--;
	if(approximator[idx].confidence == -9)
	    approximator[idx].confidence = -8;
    }

    return retval.b;
}

void LVA::init() {
    for(int i = 0; i < 512; i++) {
	approximator[i].degree = 0;
	approximator[i].confidence = -8; // this ensures that the predictor won't be used for the first 8 times
	approximator[i].LHB_head = 0;
	approximator[i].tag = 0;
    }

    GHB[0].b = 0;
    GHB[1].b = 0;
    GHB[2].b = 0;
    GHB[3].b = 0;

    atexit(print_summary);

    init_done = true;
}

void LVA::print_summary() {
    printf("LVA accesses:\t\t%lld\n", stats_accesses);
    printf("LVA cache misses:\t%lld\n", stats_cache_misses);
    printf("LVA predictions:\t%lld\n", stats_predictions);
}