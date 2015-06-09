#!/usr/bin/env python

# script for parallelizing PERFECT suite error injection experiments on cluster

# inputs:
# output directory: directory where all experiment output directories are created

import sys
import os
import subprocess

import cw.client
import cw.slurm

import genConfigs
import perfectExperiments


# generate set of configuration files for REACT for kernel
# needs to be run once per set of experiments for kernel
def buildUsage(kernel, cfindir, cfoutdir):
    if not os.path.exists(cfindir):
        os.makedirs(cfindir)
        genConfigs.genConfigs(kernel, cfindir)
    if not os.path.exists(cfoutdir):
        os.makedirs(cfoutdir)
    # run buildUsage

# invoke run-kernel.sh
def runexp(app, kernel, config, outdir, ofname, ifname, runOctave):
    intRunOctave = 1 if runOctave else 0
    if ifname == None:
        print('{0} {1} {2} {3} {4} {5}'.format(app,kernel,config,outdir,ofname,intRunOctave))
        subprocess.call(["./run-kernel.sh", app, kernel, config, outdir, ofname, str(intRunOctave)])
    else: 
        print('{0} {1} {2} {3} {4} {5} {6}'.format(app,kernel,config,outdir,ofname,intRunOctave,ifname))
        subprocess.call(["./run-kernel.sh", app, kernel, config, outdir, ofname, str(intRunOctave), ifname])

def completion(jobid, output):
    print(u'finished running job {0}'.format(jobid))


# main routine: run all experiments
# args:
# 1. indir: input configuration directory
def main(indir):
    
    print(indir)

    # kernels describes the app, kernel, input, output, precise-output, for each kernel
    kernels = perfectExperiments.getKernels();

    for (a,k,ifname,ofname,runOctave) in kernels:
        print('{0} {1} {2} {3} {4}'.format(a,k,ifname,ofname,runOctave))

        # generate path for base output directory for this app/kernel
        kindir = os.path.join(os.path.join(indir, a), k)

        # generate path for configuration files & build configuration files
        cfindir = os.path.join(kindir, 'inject_configs')
        cfoutdir = os.path.join(kindir, 'react_configs')
        buildUsage(k,cfindir,cfoutdir)
        

def usage():
    print('usage: python run.py outdir nworkers')
    print('indir: base input directory')
    exit(0)

if __name__ == "__main__":
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        usage()

