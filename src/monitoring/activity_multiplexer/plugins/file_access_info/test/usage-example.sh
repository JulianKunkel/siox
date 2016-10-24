#!/bin/bash

# A small SIOX-FileAccessInfoPlugin tutorial
# Author: Eugen Betke (betke@dkrz.de)


### PREPARATION ###
# CHANGE THIS PATH #
INSTALL="/work/ku0598/k202107/git/siox/siox-access_info_plotter/install/"


# Convinience variables are not necessary, when siox is properly installed.
INST="$INSTALL/bin/siox-inst"
TRACEREADER="$INSTALL/bin/siox-trace-reader"
LD_LIBRARY_PATH="$INSTALL/lib64:$LD_LIBRARY_PATH"
TESTPROG="w"

# Clean up
rm activities.dat*

### STEP 1 ###
# siox-inst creates for each process an activity file. Depending on application you can get a number of files.
$INST posix $TESTPROG


### STEP 2 ###
# siox-trace-reader processes one single file at a time, therefore we need a list of files.
FILES=($(find -regextype posix-egrep  -regex './activities.dat[[:digit:]]*'))


for FILE in ${FILES[@]}
do
	# All supported options you can list with the following command:
	# siox-trace-reader --help

	# Apply our plugin to each file in the list.
	${TRACEREADER} --FileAIStreamOptions.filename $FILE --FileAccessInfoPluginOptions.output $FILE.txt
done

