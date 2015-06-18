sourced=$_

# Source this script (don't execute it).
if [ -z "$sourced" -o "$sourced" == "/bin/sh" ] ; then
    echo "please source this script"
fi

# set ACCEPT_DIR, ACCEPTAPPS_DIR for error injection
#export ACCEPT_DIR=/sampa/share/accept/accept
export ACCEPT_DIR=~/research/accept
export ACCEPTAPPS_DIR=~/research/accept-apps

# set PERFECT directory for inputs / outputs
#export PERFECT_DIR=/sampa/share/PERFECT
export PERFECT_DIR=~/research/perfect-suite-v1.0
# setup link to data for PERFECT apps
if [ ! -h $ACCEPTAPPS_DIR/perfect/data ]; then
    ln -s $PERFECT_DIR/suite $ACCEPTAPPS_DIR/perfect/data
fi

# set REACT_DIR for profiling (also need to set RUNSHIM before profiling)
export REACT_DIR=~/research/approx-limit/models/simple/

unset sourced
