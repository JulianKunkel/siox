#!/bin/bash

WRAPPER=$1
shift

CUR_DIR="$PWD"
DIR=$(dirname $0)
cd $DIR/../../

SOURCE=tools/siox-skeleton-builder/layers/$WRAPPER

if [[ "$WRAPPER" == "" || ! -d "$SOURCE" ]] ; then
	echo "SYNTAX: $0 [WRAPPER] [CONFIGURE OPTIONS]"
	exit 1
fi

pushd $SOURCE
rm -rf build 2>/dev/null
./waf configure install $@ || exit 1
popd
