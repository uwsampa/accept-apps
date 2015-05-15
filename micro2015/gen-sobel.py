#!/usr/bin/python

import sys
import os

def dump(outdir,base,model,mparam):
    name=os.path.join(outdir,base+'-'+str(mparam)+'.txt')
    f = open(name,'w')
    f.write('default 0\n')
    f.write('sobel_filtering '+str((model<<16) | mparam)+'\n')
    f.close()

if __name__ == "__main__":

    outdir = sys.argv[1]

    # dump EnerJ configs
    bytes=range(1,9)
    level=[1,2,3]
    for b in bytes:
        for l in level:
            mparam = (100 + b*10 + l) # no mem, alu
            dump(outdir,'enerj',1,mparam)
            mparam = (1000 + b*10 + l) # mem, no alu
            dump(outdir,'enerj',1,mparam)
            mparam = (1100 + b*10 + l) # mem and alu
            dump(outdir,'enerj',1,mparam)

    # dump Flikker
    dump(outdir,'flikker',3,1)
    dump(outdir,'flikker',3,2)

    # dump ReducedPrecFP
    dump(outdir,'reducedprecfp',2,8)
    dump(outdir,'reducedprecfp',2,16)

