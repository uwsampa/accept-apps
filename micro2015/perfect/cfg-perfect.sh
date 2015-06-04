sourced=$_

# Source this script (don't execute it).
if [ -z "$sourced" -o "$sourced" == "/bin/sh" ] ; then
    echo "please source this script"
fi

# set ACCEPT_DIR, ACCEPTAPPS_DIR, RUNSHIM
export ACCEPT_DIR=/sampa/share/accept/accept
export ACCEPTAPPS_DIR=~/research/accept-apps

unset sourced
