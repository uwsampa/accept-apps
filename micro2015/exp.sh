#!/bin/bash

# acccept-apps to run experiments on
APPS=(sobel)
#blackscholes jpeg)
# specify output file names
sobel=(out.pgm)
blackscholes=(output.txt)
jpeg=(baboon-220px.rgb.jpg)

ACCEPTAPPS_DIR=~/research/accept-apps
ACCEPT_DIR=~/research/accept

OUTDIR=exp-res

# main loop
# process each app directory under accpet-apps
libfiles=$ACCEPTAPPS_DIR/liberror/*
echo $libfiles
# copy liberror files to directory
for a in ${APPS[@]}
do
    # move into app directory
    cd $ACCEPTAPPS_DIR/$a

    cp $libfiles .

    echo "build_orig"
    make build_orig

    if [ ! -d $OUTDIR ]; then
        mkdir $OUTDIR
    fi

    outfiles="$a[0]"
    outfile=${!outfiles[0]}

    # get inject_config.txt files
    configs=configs/*
    echo $configs
    for cf in $configs
    do
        cp $cf inject_config.txt
        # copy paramenters from inject_config.txt to accept_config.txt
        $ACCEPT_DIR/bin/inject_config.py
        make run_opt
        # save output file
        ext="${outfile##*.}"
        cfname="${cf##*/}"
        cfname="${cfname%.*}"
        fname=$cfname.$ext
        echo $fname
        mv $outfile $OUTDIR/$fname
    done

    # cleanup libfiles
    echo "removing libfiles..."
    echo ""
    for lfpath in ${libfiles[@]}
    do
        lf="${lfpath##*/}"
        rm $lf
    done

    make clean

    # change back to previous directory
    cd -
done