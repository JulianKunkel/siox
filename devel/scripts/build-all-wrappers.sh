#!/bin/bash

SIOX=/opt/siox/Inst/
CLEAN="$1"

(
ERROR=""
for L in /opt/siox/siox/src/tools/siox-skeleton-builder/layers/*; do
	pushd $L
	if [[ $CLEAN != "" ]] ; then
		rm -rf build
	fi
	./waf configure --siox=$SIOX --prefix=$SIOX 2>&1
	./waf install || ERROR="$ERROR $(basename $L)"
	popd
done
echo $ERROR > error-wrappers.log
) 2>&1 | tee wrappers.log

ERROR=$(cat error-wrappers.log)
echo ""
if [[ $ERROR != "" ]] ; then
	echo "Error while processing: $ERROR"
	echo "The configure/compiler output is stored in wrappers.log"
fi
