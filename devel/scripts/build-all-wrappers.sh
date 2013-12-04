#!/bin/bash

SIOX="$1"
CLEAN="$2"

CUR_DIR="$PWD"
DIR=$(dirname $0)
cd $DIR/../../

if [[ "$SIOX" == ""  || ! -d "$SIOX" ]] ; then
	echo "SYNTAX: $0 <SIOX-INSTALL-PATH> [CLEAN]"
	echo "If clean is set, then ./waf clean install is called"
	exit 1
fi

for L in tools/siox-skeleton-builder/layers/*; do
	NAME=$(basename $L)
	if [[ ! -d $L ]] ; then
		continue
	fi
	rm $CUR_DIR/$NAME.err 2>/dev/null
	pushd $L
	if [[ $CLEAN != "" ]] ; then
		rm -rf build
	fi
	( 
	./waf configure --siox=$SIOX --prefix=$SIOX 
	if [ "$?" != 0 ] ; then
		touch $CUR_DIR/$NAME.err
		continue
	fi
	./waf install 2>&1 || touch $CUR_DIR/$NAME.err
	) 2>&1 | tee $CUR_DIR/$NAME.log 
	popd
done

ERROR=$(ls *.err 2>/dev/null | sed "s/.err//g" || echo "" )
echo ""
if [[ $ERROR != "" ]] ; then
	echo "Error while processing:"
	echo $ERROR
	echo "The configure/compiler output is stored in <LAYER>.log"
else
	echo "All wrappers are sucessfully installed!"
fi
