#!/bin/bash -ex

# export LD_LIBRARY_PATH=/usr/local/siox/lib/
# export PATH=/usr/local/siox/bin:$PATH

echo "Running daemon"
rm *.dat /tmp/daemon.socket 2>/dev/null || true
killall siox-daemon || true

siox-daemon --configEntry=daemon-withoutStats.conf &
# siox-daemon --configEntry=daemon.conf &

echo "Compiling"
siox-inst posix wrap gcc -g -Wall fwrite.c -o fwrite-wrapped
gcc -g -Wall fwrite.c -o fwrite
mpicc test.c -Wall -g -o test
siox-inst posix,mpi wrap mpicc test.c -Wall -g -o test-wrapped

echo "Running the APP with SIOX instrumentation to create ontology etc."
siox-inst posix ./fwrite
siox-inst mpi ./test

echo "Running the APP with SIOX instrumentation"
mpiexec.mpich2 -np 4 siox-inst posix ./test

echo "To prepare IOR adjust it's Makefile after running ./configure"
echo 'sed -i "s/mpicc/siox-inst -v posix,mpi wrap mpicc/" src/Makefile'
echo "Running IOR"

echo Without wrapper run: mpiexec -np 4 --env=LD_PRELOAD="/usr/local/siox/lib/libsiox-mpi-dlsym.so /usr/local/siox/lib/libsiox-posix-dlsym.so" ...

export SIOX_REPORTING_FILENAME=ior-report.txt
mpiexec -np 4 ./ior.wrapped -v -o testfile -s 10 -i 1 -f ior.conf
mpiexec -np 4 -env=LD_PRELOAD="/usr/local/siox/lib/libsiox-mpi-dlsym.so /usr/local/siox/lib/libsiox-posix-dlsym.so"  ./ior.normal -v -o testfile -s 10 -i 1 -f ior.conf


echo "Run completed!"


echo
echo "Trace output"

siox-trace-reader


killall siox-daemon || true
