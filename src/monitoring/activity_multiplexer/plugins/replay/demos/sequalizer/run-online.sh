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

# prepare directory
mkdir $TRACE_DIR
cd $TRACE_DIR

# symling siox.conf and create trace
ln -s ../siox-online-modify.conf siox.conf

# TODO add other reader file.
ln -s ../siox-trace-reader.conf siox-trace-reader.conf

cp ../*.dat .
siox-inst posix-deedless ../program

# symlink activities.dat1234 for convienience
ACTIVITIES=`find . -name "activities.dat[0-9]*" | tail -n 1`
echo ${ACTIVITIES}
ln -s $ACTIVITIES activities.dat
