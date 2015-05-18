#!/bin/bash

# this script runs error injection experiments
# a gen-app.py script must be present for each app to be run

WORKING_DIR=`pwd`
CONFIG_DIR=$WORKING_DIR/configs
OUTDIR=$WORKING_DIR/results

# base directory for accept-apps and accept repos
# ACCEPT_BASE=~/research
ACCEPTAPPS_DIR=$ACCEPT_BASE/accept-apps
ACCEPT_DIR=$ACCEPT_BASE/accept

# acccept-apps to run experiments on
APPS=(sobel)
#(sobel blackscholes jpeg)
# specify output file names
sobel=(out.pgm)
blackscholes=(output.txt)
jpeg=(baboon.rgb.jpg)

if [ ! -d $ACCEPT_BASE ]; then
   echo "Please set the ACCEPT_BASE to point to the root of the repo:"
   echo "    e.g. export ACCEPT_BASE=~/accept"
   exit 0
fi

# create config directory
if [ ! -d $CONFIG_DIR ]; then
    mkdir $CONFIG_DIR
fi

# main loop
# process each app specified from accpet-apps
libfiles=$ACCEPTAPPS_DIR/liberror/*
# copy liberror files to directory
for a in ${APPS[@]}
do
    echo "processing $a..."

    # generate all configuration files
    if [ ! -d $CONFIG_DIR/$a ]; then
        mkdir -p $CONFIG_DIR/$a
    fi
    python gen-configs.py $a $CONFIG_DIR/$a

    # make output directory for this app
    if [ ! -d $OUTDIR/$a ]; then
        mkdir -p $OUTDIR/$a
    fi

    # get name out output file for this app
    outfiles="$a[0]"
    outfile=${!outfiles[0]}

    # move into app directory
    cd $ACCEPTAPPS_DIR/$a

    # copy library files into directory
    cp $libfiles .

    # run build_orig to generate accept_config.txt file
    make build_orig

    # get inject_config.txt files for this app
    configs=$CONFIG_DIR/$a/*

    echo ""
    echo "Configs to evaluate..."
    echo $configs

    # run an experiment for each config file present
    for cf in $configs
    do
        echo "config $cf"
        # rename a copy of the file to inject_config.txt, as expected by accept
        cp $cf inject_config.txt
        # copy paramenters from inject_config.txt to accept_config.txt
        $ACCEPT_DIR/bin/inject_config.py
        # run the experiment
        make run_opt
        # save output file, first 4 commands generate a properly named output file
        ext="${outfile##*.}"
        cfname="${cf##*/}"
        cfname="${cfname%.*}"
        fname=$cfname.$ext
        echo "saving output file to $fname"
        mv $outfile $OUTDIR/$a/$fname
    done

    # cleanup libfiles
    echo "removing libfiles..."
    echo ""
    for lfpath in ${libfiles[@]}
    do
        lf="${lfpath##*/}"
        rm $lf
    done

    # cleanup
    make clean

    # change back to previous directory
    cd -
done