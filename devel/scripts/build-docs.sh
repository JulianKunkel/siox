#!/usr/bin/env bash
# Small script to generate the SIOX documentation.

# If you don't have the needed programs in your $PATH, you may adjust here
DOXYGEN=doxygen
PLANTUML=plantuml

# Where to search for PlantUML code
PLANTUML_SOURCES="doc/doc-source/ src/"


# Determine SIOX root directory
cd "$(dirname "$0")"
cd ../..
SIOX_ROOT="$(pwd)"
# Export for use in Doxyfile
export SIOX_ROOT
printf "SIOX root directory is '$SIOX_ROOT'\n"

which "$DOXYGEN" &>/dev/null || { printf "\nCannot find doxygen!\n\n"; exit 1; }
which "$PLANTUML" &>/dev/null || {
	printf "\nCannot find plantuml!\n\n"
	if [ ! -e ~/bin/plantuml ]; then
		if [ ! -d ~/bin -a ! -e ~/bin ]; then
			mkdir ~/bin
		fi
		cat >~/bin/plantuml <<EOT
#!/bin/sh
exec java -jar /path/to/plantuml.jar "\$@"
EOT
		chmod +x ~/bin/plantuml
		printf "A wrapper for invoking plantuml.jar has been created for your convenience at '~/bin/plantuml'\nAdjust the path to the jar file and try again.\n"
	fi
	exit 1
}

# Generate PlantUML diagrams
cd "$SIOX_ROOT"
printf "Searching for PlantUML source files in $PLANTUML_SOURCES ...\n"
grep -RI --files-with-matches '^[[:space:]]*@startuml' $PLANTUML_SOURCES | while read filename
do
	printf "Generating UML from '$filename' ...\n"
	"$PLANTUML" -o "$SIOX_ROOT/doc/doc-created/images" "$filename" || { printf "\nPlantUML error!\n\n"; exit 1; }
done

[ $? -ne 0 ] && exit 1

# Generate Doxygen documentation
cd "$SIOX_ROOT/doc" && "$DOXYGEN"
