#!/bin/bash

REACT_HOME=~/research/approx-limit

if ! (( ($# == 2) )); then
    echo "Invalid number of arguments"
    echo "./run-build-usage.sh App Kernel"
    exit 0
fi

APP=$1
KERNEL=$2

PERFECT_EXP=~/research/accept-apps/micro2015/perfect/experiments/$APP/$KERNEL

shopt -s nullglob
CONFIGS=$PERFECT_EXP/inject_configs/*
OUT_DIR=$PERFECT_EXP/react_configs

formatAndMove ()
{
    xmllint --format $1 > tmp.xml && mv tmp.xml $1
    mv $1 $OUT_DIR/$1
}

# Ensure that paths are correctly set
if [ ! -d $REACT_HOME ]; then
   echo "Please set the REACT_HOME to point to the root of the repo:"
   echo "    e.g. export REACT_HOME=~/approx-limit"
   exit 0
fi

if [ ! -d "$OUT_DIR" ]; then
    mkdir -p $OUT_DIR
fi

for f in $CONFIGS
do
    echo $f
    python $REACT_HOME/models/simple/build-usage.py $f
done

for f in ./*.xml
do
    formatAndMove $f
done
