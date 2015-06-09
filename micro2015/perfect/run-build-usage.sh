#!/bin/bash

REACT_HOME=~/research/approx-limit

shopt -s nullglob
CONFIGS=inject_configs/*
OUT_DIR=react_configs

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