aptitude install libpqxx3-dev libpqxx-3.1-dbg

To build and test the POSIX wrapper use:

cd $SIOX

INSTALL=/usr/local/siox

./waf configure  --prefix=$INSTALL
./waf install


cd tools/siox-skeleton-builder/layers/posix
./waf configure --siox=/usr/local/siox --prefix=$INSTALL
./waf

Then you can run tests, for example:
LD_PRELOAD=build/libsiox-posix-dlsym.so dd if=/dev/zero of=/tmp/test count=1024 bs=1000

These files could be visualized using
$INSTALL/bin/siox-trace-reader
