#!/usr/bin/env python

# script for parallelizing PERFECT suite error injection experiments on cluster

# inputs:
# output directory: directory where all experiment output directories are created

import sys
import os
import subprocess

import cw.client
import cw.slurm


def completion(jobid, output):
    print(u'finished running job {0}'.format(jobid))

# main routine: run all experiments
# args:
# 1. outdir: output directory for all experiments
def main():
    
    # fire up worker cluster
    # cw.slurm.start(nworkers=n)
    # setup the client
    client = cw.client.ClientThread(completion, cw.slurm.master_host())
    
    # wait for all jobs to finish
    client.wait()

    # shut down worker cluster
    cw.slurm.stop()


if __name__ == "__main__":
    main()
