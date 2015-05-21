#!/usr/bin/python

import sys
import os

# Defines
coarseEnable = True
coarseDisable = False

# Approximation model mapping
approxModelMap = {
    # fine grained techniques 
    'precise' : 0,
    'enerj': 1,
    'reducedprecfp': 2,
    'flikker': 3,
    'lva': 4,
    'fuzzymemoifp': 5,
    'fuzzymemoiint': 6,
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
    'fft-1d' : ['fft']
}

def dump(bench,outdir,base,mparam,coarse):
    model=approxModelMap[base]
    name=os.path.join(outdir,base+'-'+str(mparam)+'.txt')
    f = open(name,'w')
    if (coarse):
        f.write('coarse ' + ' ' + str((model<<16) | mparam) + '\n')
        f.close()
    else:
        f.write('coarse 0\n')
        f.write('default 0\n')
        for phase in benchmarkPhases[bench]:
            f.write(phase + ' ' + str((model<<16) | mparam) + '\n')
        f.close()

if __name__ == "__main__":

    bench = sys.argv[1]
    outdir = sys.argv[2]

    # dump precise
    dump(bench,outdir,'precise',0,False)

    # dump EnerJ configs
    bytes=range(1,9)
    level=[1,2,3]
    for b in bytes:
        for l in level:
            mparam = (100 + b*10 + l) # no mem, alu
            dump(bench,outdir,'enerj',mparam,coarseDisable)
            mparam = (1000 + b*10 + l) # mem, no alu
            dump(bench,outdir,'enerj',mparam,coarseDisable)
            mparam = (1100 + b*10 + l) # mem and alu
            dump(bench,outdir,'enerj',mparam,coarseDisable)

    # dump Flikker
    dump(bench,outdir,'flikker',1,coarseDisable)
    dump(bench,outdir,'flikker',2,coarseDisable)

    # dump lva
    dump(bench,outdir,'lva', 0x0000,coarseDisable)
    dump(bench,outdir,'lva', 0x4000,coarseDisable)
    dump(bench,outdir,'lva', 0x8000,coarseDisable)
    dump(bench,outdir,'lva', 0xC000,coarseDisable)

    # dump ReducedPrecFP
    dump(bench,outdir,'reducedprecfp',8,coarseDisable)
    dump(bench,outdir,'reducedprecfp',16,coarseDisable)

    # dump Neural Acceleration
    #dump(bench,outdir,'dnpu',npuMap[bench],coarseEnable)
    #dump(bench,outdir,'anpu',npuMap[bench],coarseEnable)

