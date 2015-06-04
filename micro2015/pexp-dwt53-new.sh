#!/bin/bash

APP=(dwt53)

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
configs=( $CONFIG_DIR/* )
#echo ${configs[@]}

# run an experiment for each config file present

lencfg=${#configs[@]}
echo $lencfg
BATCH_SIZE=12

# run an experiment for each config file present, but dispatch in batches
exp_exe=0
while [ $exp_exe -lt $lencfg ]
do
    # determine number of experiments to run on this sbatch invocation
    # run up to BATCH_SIZE experiments per sbatch invocation
    exp_to_run=$(($lencfg-$exp_exe))
    if [ $exp_to_run -gt $BATCH_SIZE ]; then
	exp_to_run=$BATCH_SIZE
    fi

    startidx=$(($exp_exe))
    endidx=$(($startidx+$exp_to_run-1))
    echo "running batch from idx: $startidx to $endidx"

    # build an sbatch script containing up to BATCH_SIZE tasks
    tempsb=$APP-$startidx-$endidx.sb
    echo $tempsb
    touch $tempsb
    echo "#!/bin/bash" > $tempsb
    echo "#SBATCH -N 1" >> $tempsb
    echo "" >> $tempsb
    
    for (( i=$startidx; i<=$endidx; i++ ))
    do
	cf=${configs[$i]}
	echo "$WORKING_DIR/run-dwt53.sh $cf $APP $APP_DIR $OUTDIR $ACCEPT_DIR $ACCEPTAPPS_DIR &> $APP.$i.log &> $OUTDIR/$APP-$cf.log &" >> $tempsb
    done

    # invoke sbatch on the collection of jobs
    sbatch --job-name=$USER-$JOB_SET_ID $tempsb

    # increment number of experiments dispatched
    exp_exe=$(($exp_exe+$exp_to_run))
done