# Install all required packages for SIOX on an Ubuntu system.
aptitude update
aptitude upgrade

apt-get -y install g++ python3 libdb6.0-dev libboost-dev libboost-program-options-dev libboost-serialization-dev libboost-regex-dev libboost-system-dev libboost-random-dev libboost-thread-dev ninja-build cmake libglib2.0-dev libpqxx3-dev postgresql-common 

# The following is needed for testing.

# setup git and ccmake (optional)
apt-get install -y git cmake-curses-gui

# prepare SIOX
git clone https://github.com/JulianKunkel/siox.git

# update SIOX:
cd siox
git pull

rm -rf build
./configure --use-ninja --with-libpq-type-dir=/usr/share/postgresql-common/t/

cd build
ninja || exit 1

rm -rf /usr/local/*
ninja install

sed "s/Site:.*/Site: chroot-$0-ninja/" DartConfiguration.tcl
ctest -D Nightly 
