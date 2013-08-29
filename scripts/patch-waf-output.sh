#!/bin/bash
# This will search for a waf script in the current, or a parent directory, and patch it if it has already been executed.
(
	pushd . > /dev/null
	cd ${0%/*}
	PATCH_PATH=$(pwd)/waf_unit_test_output.patch
	popd > /dev/null
	until [[ -e waf ]] ; do
		if [[ $(pwd) == "/" ]] ; then
			echo "Error: Couldn't find a waf script."
			exit
		fi
		cd ..
	done
	if [[ $(echo .waf-*) == ".waf-*" ]] ; then
		echo "Error: You have to execute ./waf before I can patch it."
		exit
	fi
	patch  -p0  < $PATCH_PATH
)
