#!/bin/bash

cf=$1
APP=$2
ACCEPTAPPS_DIR=$3
OUTDIR=$4
outfile=$5
ACCEPT_DIR=$6

echo $1
echo $2
echo $3
echo $4
echo $5

ext="${outfile##*.}"
cfname="${cf##*/}"
cfname="${cfname%.*}"

echo "config $cf"
    # move into app directory

mkdir $ACCEPTAPPS_DIR/$APP-$cfname
cp $ACCEPTAPPS_DIR/$APP/* $ACCEPTAPPS_DIR/$APP-$cfname/
cd $ACCEPTAPPS_DIR/$APP-$cfname
ls

make clean

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
        # save output file, first 4 commands generate a properly named output file

fname=$cfname.$ext
echo "saving output file to $fname"

mv $outfile $OUTDIR/$fname

    # change back to previous directory
cd -

rm -r $ACCEPTAPPS_DIR/$APP-$cfname