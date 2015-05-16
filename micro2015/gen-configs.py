#!/usr/bin/python

import sys
import os

# Approximation model mapping
approxModelMap = {
    'enerj': 1,
    'reducedprecfp': 2,
    'flikker': 3,
    'lva': 4,
    'fuzzymemoifp': 5,
    'fuzzymemoiint': 6
}

benchmarkPhases = {
    'blackscholes': ['_Z19BlkSchlsEqEuroNoDivfffffif', '_Z4CNDFf'],
    'jpeg': ['dct', 'quantization', 'levelShift'],
    'sobel': ['sobel_filtering']
}

def dump(bench,outdir,base,mparam):
    model=approxModelMap[base]
    name=os.path.join(outdir,base+'-'+str(mparam)+'.txt')
    f = open(name,'w')
    f.write('default 0\n')
    for phase in benchmarkPhases[bench]:
        f.write(phase + ' ' + str((model<<16) | mparam) + '\n')
    f.close()

if __name__ == "__main__":

    bench = sys.argv[1]
    outdir = sys.argv[2]

    # dump EnerJ configs
    bytes=range(1,9)
    level=[1,2,3]
    for b in bytes:
        for l in level:
            mparam = (100 + b*10 + l) # no mem, alu
            dump(bench,outdir,'enerj',mparam)
            mparam = (1000 + b*10 + l) # mem, no alu
            dump(bench,outdir,'enerj',mparam)
            mparam = (1100 + b*10 + l) # mem and alu
            dump(bench,outdir,'enerj',mparam)

    # dump Flikker
    dump(bench,outdir,'flikker',1)
    dump(bench,outdir,'flikker',2)

    # dump ReducedPrecFP
    dump(bench,outdir,'reducedprecfp',8)
    dump(bench,outdir,'reducedprecfp',16)

