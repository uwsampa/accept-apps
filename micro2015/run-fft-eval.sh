#!/bin/bash

WORKING_DIR=`pwd`

FFT_DIR=~/research/accept-apps/micro2015/fft-1d
OUTPUTS=$FFT_DIR/outputs/*
INPUT_DIR=$FFT_DIR/inputs

for of in $OUTPUTS
do

    cp $of $WORKING_DIR/fft_output.mat
    ifname="${of##*/}"
    ifname="${ifname%.*}"
    cp $INPUT_DIR/$ifname-input.mat $WORKING_DIR/random_input.mat

    echo $ifname

    octave -q assess.m

    rm -f $WORKING_DIR/fft_output.mat
    rm -f $WORKING_DIR/random_input.mat

done