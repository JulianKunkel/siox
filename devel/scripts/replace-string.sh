#!/bin/bash

NEEDLE="$1"
FILE_EXTENSION="$2"
REPLACE="$3"
DRY_RUN="$4"

if [[ "$DRY_RUN" == "" ]] ; then
flags=-i
fi

echo "Searching for $NEEDLE in *.$FILE_EXTENSION and replace by $REPLACE"
for I in $(grep -r "$NEEDLE" $(find -name "$FILE_EXTENSION")  | cut -d ":" -f 1) ; do 
	echo "Found in $I"
	sed $flags "s#$NEEDLE#$REPLACE#g" $I
done

