#!/bin/bash -e

echo "# This script sets the LD_LIBRARY_PATH to include all shared libraries."
echo "# This is very useful for debugging of executables in build/"
echo "# Just source $0"

if [[ "$OLD_LD_LIBRARY_PATH" == "" ]] ; then
	export OLD_LD_LIBRARY_PATH=":$LD_LIBRARY_PATH"
fi
D="$OLD_LD_LIBRARY_PATH"
for X in  `(for I in $(find build -name "*.so") ; do dirname $I ; done)|uniq` ;  do
D="$D:$PWD/$X"
done

echo "export LD_LIBRARY_PATH=$D"
export LD_LIBRARY_PATH=$D

export G_SLICE=always-malloc
export G_DEBUG=gc-friendly

