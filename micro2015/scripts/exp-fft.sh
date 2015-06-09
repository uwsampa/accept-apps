#!/bin/bash

APP=(fft-1d)

WORKING_DIR=`pwd`
CONFIG_DIR=$WORKING_DIR/$APP/inject_configs
OUTDIR=$WORKING_DIR/$APP/outputs
INPUT_DIR=$WORKING_DIR/$APP/inputs

# base directory for accept-apps and accept repos
ACCEPT_BASE=~/research
ACCEPTAPPS_DIR=$ACCEPT_BASE/accept-apps
APP_DIR=$ACCEPTAPPS_DIR/perfect/required/kernels

# if on cluster, point to shared accept install
ACCEPT_DIR=/sampa/share/accept/accept
#ACCEPT_DIR=$ACCEPT_BASE/accept

if [ ! -d $ACCEPT_BASE ]; then
   echo "Please set the ACCEPT_BASE to point to the root of the repo:"
   echo "    e.g. export ACCEPT_BASE=~/accept"
   exit 0
fi

# main loop
# process each app specified from accpet-apps
libfiles=$ACCEPTAPPS_DIR/liberror/*

echo "processing $APP..."

# generate all configuration files
if [ ! -d $CONFIG_DIR ]; then
    mkdir -p $CONFIG_DIR
    python $ACCEPTAPPS_DIR/micro2015/gen-configs.py $APP $CONFIG_DIR
fi

# make output directory for this app
if [ ! -d $OUTDIR ]; then
    mkdir -p $OUTDIR
fi

# make inputs directory for this app
if [ ! -d $INPUT_DIR ]; then
    mkdir -p $INPUT_DIR
fi

# move into app directory
cd $APP_DIR/$APP

# copy library files into directory
cp $libfiles .

# run build_orig to generate accept_config.txt file
make build_orig

# get inject_config.txt files for this app
configs=$CONFIG_DIR/*

# run an experiment for each config file present
for cf in $configs
do
    pwd
    echo "config: $cf"
    # rename a copy of the file to inject_config.txt, as expected by accept
    cp $cf inject_config.txt
    # copy paramenters from inject_config.txt to accept_config.txt
    $ACCEPT_DIR/bin/inject_config.py
    # run the experiment
    make run_opt
    # save output file
    cfname="${cf##*/}"
    cfname="${cfname%.*}"
    mv fft_output.mat $OUTDIR/$cfname.mat
    mv random_input.mat $INPUT_DIR/$cfname-input.mat
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