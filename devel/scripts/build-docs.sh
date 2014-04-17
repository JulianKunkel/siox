#!/usr/bin/env bash
#Small script to generate the SIOX documentation.

SIOX_ROOT=/home/aguilera/Projects/SIOX/github/siox

PUMLJAR=/home/aguilera/bin/plantuml.jar
DOXYGEN=/usr/bin/doxygen
JAVA=/usr/bin/java

pushd $SIOX_ROOT

#TODO generate plantuml diagrams
grep -RI --files-with-matches '@startuml' --exclude-dir=devel --exclude-dir=build\* --exclude=Doxyfile | while read line
do
	$JAVA -jar $PUMLJAR -o $SIOX_ROOT/doc/doc-created/images $line 
done

#Generate documentation
pushd doc
$DOXYGEN 
popd
popd


