#!/bin/bash

# This script is used by ScryMUD to move it's log files into
# a more convenient location.  If one desired, this script
# could even be made to ftp them somewhere else...


LOG_TARG_DIR=$HOME/public_html
LF=/tmp/mv_logs.log

# First, remove the existing target.

mkdir -p $LOG_TARG_DIR/World

#echo "cp $1 $LOG_TARG_DIR/" >> $LF
cp $1 $LOG_TARG_DIR/

#chmod a+r $LOG_TARG_DIR/BC_* >> $LF
