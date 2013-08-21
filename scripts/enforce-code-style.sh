#!/bin/bash
#
# enforce-code-style.sh V1.0
#
# Script to apply the SIOX code style conventions to the whole source tree
# under $SIOX/Implementation/ using the tool astyle.
# The actual options defining the coding style are set down in siox.astyle
# The file system references are such that this script is to be run from the
# Implementation directory.
#
# 2013-08-21 by Michaela Zimmer
#
echo ======================
if [ `which astyle` ]
then
	astyle --recursive --options=scripts/siox.astyle "./*.h" "./*.c" "./*.hpp" "./*.cpp"
else
	echo "This script requires the tool Artistic Style (astyle)."
	echo "Please install it and try again."
fi
echo ======================