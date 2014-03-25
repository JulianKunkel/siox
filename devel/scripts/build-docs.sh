#! /usr/bin/env bash
#Small script to generate the SIOX documentation.

#Go to SIOX base directory
cd $(dirname $(dirname $(dirname $0)))

#TODO generate plantuml diagrams
echo "scanning sources for plantuml diagrams, this may take a while if it's the first time after a reboot"
for file in $(grep --files-with-matches '@startuml' $(find . -name '*') 2>/dev/null) ; do
	case $file in
		(*Doxyfile)
			;;
		(*)
			echo "generating plantuml diagrams from $file"
			plantuml -o $(pwd)/doc/doc-created/images $file
			;;
	esac
done

#Generate documentation
cd doc
doxygen
