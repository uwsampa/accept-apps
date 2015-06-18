#!/bin/bash

# this script runs profiling on a given kernel
# this script should be invoked for each app/kernel pair

# TODO: profiling fails on the following pairs using ACCEPT
# pa1/dwt53
# wami/debayer

# args:
# 1: app
# 2: kernel

if ! (( ($# == 2) )); then
    echo "Invalid number of arguments"
    echo "./run-profiling.sh App Kernel"
    exit 0
fi

source ./cfg-perfect.sh

export RUNSHIM='~/Downloads/pin-2.14-67254-gcc.4.4.7-linux/pin.sh -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 1 -i phases.cfg -reg 1 -- '

APP=$1
KERNEL=$2

# generate path for temporary working directory
KERNEL_DIR=$ACCEPTAPPS_DIR/perfect/$APP/kernels/$KERNEL

if [ ! -d $KERNEL_DIR ]; then
    echo "Invalid App/Kernel pair; directory does not exist"
    exit 0
fi

OUTDIR=$ACCEPTAPPS_DIR/micro2015/perfect/experiments/$APP/$KERNEL/raw
if [ ! -d $OUTDIR ]; then
    mkdir -p $OUTDIR
fi

cd $KERNEL_DIR

make build_orig
make run_orig
mv phase-profile.csv* $OUTDIR/
make clean
rm -f pin.log

cd -

