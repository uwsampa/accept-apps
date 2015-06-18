#!/bin/bash

# TODO: factor out functionality to operate on a given app/kernel/configuration tuple

APPS=(pa1 sar stap wami required)
pa1=(2d_convolution histogram_equalization)
#dwt53
sar=(bp pfa-interp1 pfa-interp2)
stap=(inner-product outer-product system-solve)
wami=(change-detection lucas-kanade)
#debayer
required=(fft-1d fft-2d)

formatAndMove ()
{
    # app, kernel, outdir
    local a=$1
    local k=$2
    local outdir=$3
    xmllint --format $k.xml > tmp.xml && mv tmp.xml $outdir/$a-$k.xml
    rm $k.xml
    xmllint --format $k-usage.xml > tmp.xml && mv tmp.xml $outdir/$a-$k-usage.xml
    rm $k-usage.xml
    #mv default.csv $outdir/$a-$k-default.csv
}

process ()
{
    # app, kernel
    local a=$1
    local k=$2
    ~/research/approx-limit/profiling/pin/scripts/compute-phase-profile.py ~/research/accept-apps/micro2015/perfect/experiments/$a/$k/raw/ ~/research/approx-limit/profiling/pin/opcodes/opcode_map_energy.csv phase-profile.csv $k ~/research/accept-apps/perfect/$a/kernels/$k/phases.cfg
}

PERFECT_EXP=~/research/accept-apps/micro2015/perfect/experiments

for a in ${APPS[@]}
do
    kernel="$a[@]"
    for k in ${!kernel}
    do
        echo "processing $a/$k..."
        OUTDIR=$PERFECT_EXP/$a/$k/profile
        if [ ! -d $OUTDIR ]; then
            mkdir -p $OUTDIR
        fi
        process $a $k
        formatAndMove $a $k $OUTDIR
    done
done
