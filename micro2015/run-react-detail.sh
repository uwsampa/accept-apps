#!/bin/bash

if [ $# -ne 1 ]; then
    echo "./run-react.sh <output-directory>"
    exit
fi

WORKING_DIR=`pwd`
REACT_DIR=~/research/approx-limit/models/simple
REACT=$REACT_DIR/react-energy
ARCH_FILE=$REACT_DIR/archspec/arma9.xml
MODEL_DIR=$REACT_DIR/compiled_models
PROFILE_DIR=$WORKING_DIR/profiles
USAGE_DIR=$WORKING_DIR/react_configs
OUT_DIR=$1

shopt -s nullglob
PROFILES=$PROFILE_DIR/*
CONFIGS=$USAGE_DIR/*

if [ ! -d "$OUT_DIR" ]; then
    mkdir -p $OUT_DIR
fi

for p in $PROFILES
do
    echo $p
    for cf in $CONFIGS
    do
        echo $cf
        fname=`basename $cf`
        fbase="${fname%%.*}"
        $REACT $ARCH_FILE $p $cf $MODEL_DIR/
        mv summary.csv $OUT_DIR/$fbase-summary.csv
        mv summary-detail.csv $OUT_DIR/$fbase-summary-detail.csv
    done
done