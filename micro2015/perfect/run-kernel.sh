#!/bin/bash

# this script runs a single experiment on a given kernel

# the goal is to make this script invokable through Adrian's Cluster-Workers tool

# args:
# 1: app
# 2: kernel
# 3: configuration file (full file path)
# 4: kernel specific output directory (e.g.: .../experiments/pa1/dwt53
# 5: kernel specific output file name
# 6: output evaluation (1 = octave, 0 = cat)
# 7: kernel specific input file name

if ! (( ($# == 6) || ($# == 7) )); then
    echo "Invalid number of arguments"
    echo "./run-kernel.sh App Kernel Config OutputDirectory KernelOutputFileName RunOctave [KernelInputFileName]"
    exit 0
fi

source ./cfg-perfect.sh

APP=$1
KERNEL=$2
cf=$3
OUTDIR=$4
OUTFILE=$5
RUNOCTAVE=$6
INFILE=

if [ $# -eq 7 ]; then
    INFILE=$7
fi

# make paths absolute
cf=`readlink -m $cf`

if [ ! -e $cf ]; then
    echo "Invalid configuration file (does not exist)"
    exit 0
fi

# TODO: sourcing a script using the environment variables the script sets...oops
# ensure proper environment variables set (ACCEPT_DIR and ACCEPTAPPS_DIR)
#source $ACCEPTAPPS_DIR/micro2015/perfect/cfg-perfect.sh

# generate path for temporary working directory
KERNEL_DIR=$ACCEPTAPPS_DIR/perfect/$APP/kernels/$KERNEL

if [ ! -d $KERNEL_DIR ]; then
    echo "Invalid App/Kernel pair; directory does not exist"
    exit 0
fi

# copy source files to temporary working directory
cfname="${cf##*/}"
cfname="${cfname%.*}"
echo "cfname: $cfname"
TEMP_DIR=$ACCEPTAPPS_DIR/perfect/$APP/kernels/$KERNEL-$cfname
mkdir $TEMP_DIR
cp -r $KERNEL_DIR/* $TEMP_DIR/

OUTDIR=`readlink -m $OUTDIR`

# ensure output directory exists
if [ ! -d $OUTDIR ]; then
    mkdir -p $OUTDIR
fi

OFDIR=$OUTDIR/outputs
if [ ! -d $OFDIR ]; then
    mkdir -p $OFDIR
fi

IFDIR=$OUTDIR/inputs
if [ $# -eq 6 -a ! -d $IFDIR ]; then
    mkdir -p $IFDIR
fi

#echo $APP
#echo $KERNEL
#echo $cf
#echo $OUTDIR
#echo $OUTFILE
#echo $RUNOCTAVE

# ensure RUNSHIM is not set in environment
unset RUNSHIM

# enter temporary directory
cd $TEMP_DIR
# copy library files into directory
libfiles=$ACCEPTAPPS_DIR/liberror/*
cp $libfiles .

# run build_orig to generate accept_config.txt file
make build_orig

# rename a copy of the file to inject_config.txt, as expected by accept
cp $cf inject_config.txt
# copy paramenters from inject_config.txt to accept_config.txt
$ACCEPT_DIR/bin/inject_config.py
# run the experiment
make run_opt

# TODO: instead of saving output files, let's just process and generate the error metric
errfile=$cfname.err
if [ $RUNOCTAVE -eq 1 ]; then
    echo "RUNOCTAVE: running octave"
    # run Octave and store result in errfile, then save errfile
    octave -q assess.m > $errfile
    mv $errfile $OFDIR/
else
    # move and rename output file (which is just a file with error metric result)
    mv $OUTFILE $OFDIR/$errfile
fi

# change back to previous directory
cd -

rm -r $TEMP_DIR
