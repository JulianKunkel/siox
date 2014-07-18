#!/bin/bash

# this script prepares chroots for all given distributions

# Should we use: systemd-nspawn instead?

function prepareMounts(){
	CHROOT_DIR="$1"
	sudo mount -t proc proc "$CHROOT_DIR/proc/"
	sudo mount -t sysfs sys "$CHROOT_DIR/sys/"
	sudo mount -o bind /dev "$CHROOT_DIR/dev/"
	mkdir -p "$CHROOT_DIR/dev/pts"
	sudo mount -t devpts pts "$CHROOT_DIR/dev/pts/"
}

for dist in $(ls setup | grep ".sh$" ); do
	distname=${dist%%.sh}
	if [[ ! -e install/$dist ]] ; then
	  echo "could not find install script for $dist, therefore won't install it"
	  continue
	fi
	
	mkdir $distname 2>/dev/null
	cp install/$dist $distname/install.sh
	chmod 755 $distname/install.sh
	pushd $distname

	source ../setup/$dist $distname
	sudo chroot . ./install.sh
	popd
done 
