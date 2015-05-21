#!/bin/bash

# BEFORE USING: 'source /sampa/share/accept/accept/activate.sh'

# this script runs error injection experiments

# acccept-apps to run experiments on
APPS=(blackscholes sobel jpeg)
# specify output file names
sobel=(out.pgm)
blackscholes=(output.txt)
jpeg=(baboon.rgb.jpg)

# base directory for accept-apps and accept repos
ACCEPT_BASE=~/research
ACCEPTAPPS_DIR=$ACCEPT_BASE/accept-apps

# if on cluster, point to shared accept install
ACCEPT_DIR=/sampa/share/accept/accept
#ACCEPT_DIR=$ACCEPT_BASE/accept

if [[ -z $ACCEPT_BASE || (! -d $ACCEPT_BASE) ]]; then
   echo "Please set the ACCEPT_BASE to point to the root of the repo:"
   echo "    e.g. export ACCEPT_BASE=~/accept"
   exit 0
fi

if [[ ! -x $ACCEPT_DIR/bin/inject_config.py ]]; then
    echo "Cannot find '$ACCEPT_DIR/bin/inject_config.py'."
    echo "This file is necessary to run these experiments"
    echo "Please make sure that ACCEPT_DIR points to the correct place"
    exit 0
fi

WORKING_DIR=`pwd`

# main loop
# process each app specified from accpet-apps

# copy liberror files to directory
for APP in ${APPS[@]}
do
    CONFIG_DIR=$WORKING_DIR/$APP/inject_configs
    OUTDIR=$WORKING_DIR/$APP/outputs

    echo "processing $APP..."

    # generate all configuration files
    if [ ! -d $CONFIG_DIR ]; then
        mkdir -p $CONFIG_DIR
    fi
    python gen-configs.py $APP $CONFIG_DIR

    # make output directory for this app
    if [ ! -d $OUTDIR ]; then
        mkdir -p $OUTDIR
    fi

    # get name out output file for this app
    outfiles="$APP[0]"
    outfile=${!outfiles[0]}

    # get inject_config.txt files for this app
    configs=$CONFIG_DIR/*

    echo ""
    echo "Configs to evaluate..."
    echo $configs

    JOB_SET_ID=`date +%s`

    # run an experiment for each config file present
    for cf in $configs
    do

	sbatch --job-name=$USER-$JOB_SET_ID <<-EOF 
#!/bin/bash
srun ./run-aa.sh $cf $APP $ACCEPTAPPS_DIR $OUTDIR $outfile $ACCEPT_DIR
EOF

    done

done
#  LocalWords:  jpg
