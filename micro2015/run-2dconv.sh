#!/bin/bash

cf=$1
APP=$2
APP_DIR=$3
OUTDIR=$4
ACCEPT_DIR=$5
ACCEPTAPPS_DIR=$6

echo $1
echo $2
echo $3
echo $4
echo $5
echo $6

echo "config: $cf"

cfname="${cf##*/}"
cfname="${cfname%.*}"
mkdir $APP_DIR/$APP-$cfname
cp -r $APP_DIR/$APP/* $APP_DIR/$APP-$cfname/
cd $APP_DIR/$APP-$cfname
ls

make clean

# move into app directory

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
    # save output file

mv 2dconv_output.0.mat $OUTDIR/$cfname.mat

# change back to previous directory
cd -

rm -r $APP_DIR/$APP-$cfname
