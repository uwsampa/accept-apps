#!/bin/bash

APP=(histogram_equalization)

WORKING_DIR=`pwd`
CONFIG_DIR=$WORKING_DIR/$APP/inject_configs
OUTDIR=$WORKING_DIR/$APP/outputs
INPUT_DIR=$WORKING_DIR/$APP/inputs

# base directory for accept-apps and accept repos
ACCEPT_BASE=~/research
ACCEPTAPPS_DIR=$ACCEPT_BASE/accept-apps
APP_DIR=$ACCEPTAPPS_DIR/perfect/pa1/kernels

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


# get inject_config.txt files for this app
configs=$CONFIG_DIR/*

# run an experiment for each config file present
for cf in $configs
do


	sbatch --job-name=$USER-$JOB_SET_ID <<-EOF 
#!/bin/bash
srun ./run-hist.sh $cf $APP $APP_DIR $OUTDIR $ACCEPT_DIR $ACCEPTAPPS_DIR
EOF

done

