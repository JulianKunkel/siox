#!/bin/sh

# target dir
TRACE_DIR=$1
PROGRAM=$2

if [ -z "$TRACE_DIR" ];
then
	echo "Please specifiy a directory to create and store the trace in."
	echo "Example:"
	echo "$0 mytrace"
	exit
fi

# prepare directory (delete existing incase it exists to avoid conflicts)
rm -rf $TRACE_DIR
mkdir $TRACE_DIR
cd $TRACE_DIR

# symling siox.conf and create trace
ln -s ../siox.conf
ln -s ../siox-trace-replay.conf siox-trace-reader.conf
#siox-inst posix-deedless ../program
siox-inst posix ../program

# symlink activities.dat1234 for convienience
ACTIVITIES=`find . -name "activities.dat[0-9]*" | tail -n 1`
echo ${ACTIVITIES}
ln -s $ACTIVITIES activities.dat
