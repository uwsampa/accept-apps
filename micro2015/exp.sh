#!/bin/bash

# acccept-apps to run experiments on
APPS=(sobel)
#blackscholes jpeg)
# specify output file names
sobel=(out.pgm)
blackscholes=(output.txt)
jpeg=(baboon-220px.rgb.jpg)

WORKING_DIR=`pwd`
CONFIG_DIR=$WORKING_DIR/configs

# base directory for accept-apps and accept repos
BASE_DIR=~/research
ACCEPTAPPS_DIR=$BASE_DIR/accept-apps
ACCEPT_DIR=$BASE_DIR/accept

# output directory
OUTDIR=$WORKING_DIR/results

# main loop
# process each app specified from accpet-apps
libfiles=$ACCEPTAPPS_DIR/liberror/*
# copy liberror files to directory
for a in ${APPS[@]}
do
    # make output directory for this app
    if [ ! -d $OUTDIR ]; then
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
    # run an experiment for each config file present
    for cf in $configs
    do
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