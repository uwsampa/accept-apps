#!/bin/usr/env python

import sys
import os

# Defines
coarseEnable = True
coarseDisable = False

# Approximation model mapping
approxModelMap = {
    # fine grained techniques 
    'precise' : 0,
    'overscaledalu': 1,
    'reducedprecfp': 2,
    'flikker': 3,
    'lva': 4,
    'sram': 5,
    'dram': 6,
    'combo1': 7,
    'combo2': 8,

    # coarse techniques
    'dnpu' : 1,
    'anpu' : 2
}

# Maps the benchmark to the NPU model
npuMap = {
    'blackscholes': 1,
    'jpeg': 2,
    'sobel': 3
}

# Maps the benchmark to the phases
benchmarkPhases = {
    'blackscholes': ['_Z19BlkSchlsEqEuroNoDivfffffif', '_Z4CNDFf'],
    'jpeg': ['dct', 'quantization', 'levelShift'],
    'sobel': ['sobel_filtering'],
    '2d_convolution' : ['conv2d'],
    'histogram_equalization' : ['histEq'],
    'dwt53' : ['dwt53_row_transpose'],
    'fft-1d' : ['fft'],
    'fft-2d' : ['fft'],
    'inner-product' : ['cconj', 'cmult', 'complex_inner_product'],
    'outer-product' : ['cconj', 'cmult', 'stap_compute_covariance_estimate'],
    'system-solve' : ['cconj', 'cmult', 'cholesky_factorization', 'forward_and_back_substitution'],
    'bp' : ['cconj', 'cmult', 'sar_backprojection'],
    'pfa-interp1' : ['cconj', 'cmult', 'sar_interp1'],
    'pfa-interp2' : ['cconj', 'cmult', 'sinc', 'sar_interp2'],
    'change-detection' : ['wami_gmm'],
    'debayer' : ['compute_and_clamp_pixel', 'compute_and_clamp_pixel_fractional_neg', 'interp_G_at_RRR_or_G_at_BBB', 'interp_R_at_BBB_or_B_at_RRR', 'interp_R_at_GBR_or_B_at_GRB', 'interp_R_at_GRB_or_B_at_GBR', 'wami_debayer'],
    'lucas-kanade' : ['hessian', 'interpolate', 'steepest_descent', 'warp_image']
}

def dump(kernel,outdir,base,mparam,coarse):
    model=approxModelMap[base]
    
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    name=os.path.join(outdir,base+'-'+str(mparam)+'.txt')
    f = open(name,'w')
    if (coarse):
        f.write('coarse ' + ' ' + str((model<<16) | mparam) + '\n')
        f.close()
    else:
        f.write('coarse 0\n')
        f.write('default 0\n')
        for phase in benchmarkPhases[kernel]:
            f.write(phase + ' ' + str((model<<16) | mparam) + '\n')
        f.close()

def genConfigs(kernel, outdir):

    # dump precise
    #dump(kernel,outdir,'precise',0,coarseDisable)

    bytes=range(1,9) # 1..8
    level=[1,2,3]

    # generate configs for routines with byte and level constraints
    for b in bytes:
        for l in level:
            mparam = (b*10 + l) # set number of bytes and level
            # dump overscaled fpu
            dump(kernel,outdir,'overscaledalu',mparam,coarseDisable)
            # dump sram and dram configs
            dump(kernel,outdir,'sram',mparam,coarseDisable)
            dump(kernel,outdir,'dram',mparam,coarseDisable)
            # dump combo 1: overscaled alu + reduced precision fpu + sram
            dump(kernel,outdir,'combo1',mparam,coarseDisable)
            # dump combo 2: overscaled alu + reduced precision fpu
            dump(kernel,outdir,'combo2',mparam,coarseDisable)

    # generate configs for routines with only level constraint
    for l in level:
        # dump ReducedPrecFP
        dump(kernel,outdir,'reducedprecfp',l,coarseDisable)

    # dump Flikker
    dump(kernel,outdir,'flikker',1,coarseDisable)
    dump(kernel,outdir,'flikker',2,coarseDisable)

    # dump lva
    dump(kernel,outdir,'lva', 0x0000,coarseDisable)
    dump(kernel,outdir,'lva', 0x4000,coarseDisable)
    dump(kernel,outdir,'lva', 0x8000,coarseDisable)
    dump(kernel,outdir,'lva', 0xC000,coarseDisable)

    # dump Neural Acceleration
    #dump(kernel,outdir,'dnpu',npuMap[kernel],coarseEnable)
    #dump(kernel,outdir,'anpu',npuMap[kernel],coarseEnable)

if __name__ == "__main__":

    kernel = sys.argv[1]
    outdir = sys.argv[2]

    genConfigs(kernel, outdir)



