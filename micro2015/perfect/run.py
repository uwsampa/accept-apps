# script for parallelizing PERFECT suite error injection experiments on cluster

# inputs:
# output directory: directory where all experiment output directories are created

import sys
import os
import subprocess

import cw.client
import cw.slurm

from genConfigs import genConfigs

# generate set of configuration files for kernel
# needs to be run once per set of experiments for kernel
# returns path to configuration files directory
def buildConfigs(kernel, cfoutdir):
    if not os.path.exists(cfoutdir):
        os.makedirs(cfoutdir)
    genConfigs(kernel, cfoutdir)

# invoke run-kernel.sh
def runexp(app, kernel, config, outdir, ofname, ifname):
    if ifname == None:
        print('{0} {1} {2} {3} {4}'.format(app,kernel,config,outdir,ofname))
        subprocess.call(["./run-kernel.sh", app, kernel, config, outdir, ofname])
    else: 
        print('{0} {1} {2} {3} {4} {5}'.format(app,kernel,config,outdir,ofname,ifname))
        subprocess.call(["./run-kernel.sh", app, kernel, config, outdir, ofname, ifname])

def completion(jobid, output):
    print(u'finished running job {0}'.format(jobid))

# main routine: run all experiments
# args:
# 1. outdir: output directory for all experiments
def main(outdir, n):
    
    print(outdir)

    # kernels describes the app, kernel, input, and output for each kernel
    kernels = [
               ('stap','inner-product',None,'snr.txt'),
               ('stap','outer-product',None,'snr.txt'),
               ('stap','system-solve',None,'snr.txt'),
               ('sar','bp',None,'snr.txt'),
               ('sar','pfa-interp1',None,'snr.txt'),
               ('sar','pfa-interp2',None,'snr.txt'),
               ('wami','change-detection',None,'err.txt'),
               ('wami','debayer',None,'err.txt'),
               ('wami','lucas-kanade',None,'output.mat')
               ]

    # fire up worker cluster
    cw.slurm.start(nworkers=n)
    # setup the client
    client = cw.client.ClientThread(completion, cw.slurm.master_host())
    client.start()

    for (a,k,ifname,ofname) in kernels:
        print('{0} {1} {2} {3}'.format(a,k,ifname,ofname))

        # generate path for base output directory for this app/kernel
        koutdir = os.path.join(os.path.join(outdir, a), k)

        # generate path for configuration files & build configuration files
        cfoutdir = os.path.join(koutdir, 'inject_configs')
        buildConfigs(k,cfoutdir)
        
        # run one experiment per configuration
        for (dirpath, dirnames, filenames) in os.walk(cfoutdir):
            print(filenames)
            for cf in sorted(filenames):
                cffile = os.path.join(cfoutdir,cf)
                jobid = cw.randid()
                print(u'submitting job {0} on config {1}'.format(jobid, cf))
                client.submit(jobid, runexp, a,k,cffile,koutdir,ofname,ifname)

    # wait for all jobs to finish
    client.wait()

    # shut down worker cluster
    cw.slurm.stop()


def usage():
    print('usage: python run.py outdir nworkers')
    print('outdir: base output directory')
    print('nworkers: number of worker threads')
    exit(0)

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1], int(sys.argv[2]))
    else:
        usage()



"""
               ('pa1','dwt53',None,'dwt53_output.0.mat')
               ('pa1','2d_convolution',None,'2dconv_output.0.mat'),
               ('pa1','histogram_equalization',None,'histeq_output.0.mat'),
               ('required','fft-1d','random_input.mat','fft_output.mat'),
               ('required','fft-2d','random_input.mat','fft_output.mat'),
               ('stap','inner-product',None,'snr.txt'),
               ('stap','outer-product',None,'snr.txt'),
               ('stap','system-solve',None,'snr.txt'),
               ('sar','bp',None,'snr.txt'),
               ('sar','pfa-interp1',None,'snr.txt'),
               ('sar','pfa-interp2',None,'snr.txt'),
               ('wami','change-detection',None,'err.txt'),
               ('wami','debayer',None,'err.txt'),
               ('wami','lucas-kanade',None,'output.mat')
"""
