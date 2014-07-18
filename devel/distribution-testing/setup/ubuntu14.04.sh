# We expect we are running from a ubuntu system
DISTNAME="$1"


sudo debootstrap trusty ./
sudo echo "deb http://archive.ubuntu.com/ubuntu trusty main multiverse universe" > etc/apt/sources.list

