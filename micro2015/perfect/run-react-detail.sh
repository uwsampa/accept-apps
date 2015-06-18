#!/bin/bash

APPS=(pa1 sar stap wami required)
pa1=(2d_convolution histogram_equalization dwt53)
sar=(bp pfa-interp1 pfa-interp2)
stap=(inner-product outer-product system-solve)
wami=(change-detection lucas-kanade debayer)
required=(fft-1d fft-2d)

REACT_DIR=~/research/approx-limit/models/simple
REACT=$REACT_DIR/react-energy

ARCH_FILE=$REACT_DIR/archspec/arma9.xml
MODEL_DIR=$REACT_DIR/compiled_models

PERFECT_EXP=~/research/accept-apps/micro2015/perfect/experiments

for a in ${APPS[@]}
do
    kernel="$a[@]"
    for k in ${!kernel}
    do
        echo "processing $a/$k..."
        PROFILE=$PERFECT_EXP/$a/$k/profile/$a-$k.xml
        OUTDIR=$PERFECT_EXP/$a/$k/energy_results
        if [ ! -d $OUTDIR ]; then
            mkdir -p $OUTDIR
        fi

        # usage files for this app/kernel
        shopt -s nullglob
        CONFIGS=$PERFECT_EXP/$a/$k/react_configs/*
        for cf in $CONFIGS
        do
            echo $cf
            fname=`basename $cf`
            fbase="${fname%%.*}"
            $REACT $ARCH_FILE $PROFILE $cf $MODEL_DIR/

            rm summary.csv
            #mv summary.csv $OUTDIR/$fbase-summary.csv
	          # comment out the next line if detailed dump not enabled by react
            mv summary-detail.csv $OUTDIR/$fbase.csv
        done
    done
done
