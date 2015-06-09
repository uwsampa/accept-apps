#!/bin/bash

WORKING_DIR=`pwd`

HISTEQ_DIR=~/research/accept-apps/micro2015/evalruns/histogram_equalization
OUTPUTS=$HISTEQ_DIR/outputs/*

for of in $OUTPUTS
do

    cp $of $WORKING_DIR/histeq_output.0.mat
    echo $of
    octave -q assess.m

    rm -f $WORKING_DIR/histeq_output.0.mat

done