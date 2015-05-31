#!/bin/sh

# pass datadir with first argument
TRACE_DIR=$1

if [ -z "$TRACE_DIR" ]; then
	echo "Please specifiy a directory that contains a trace to replay."
	echo "Example:"
	echo "$0 mytrace"
	exit
fi

cd $TRACE_DIR
siox-trace-reader
