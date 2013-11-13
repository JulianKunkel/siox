#!/bin/bash -ex

export LD_LIBRARY_PATH=/usr/local/siox/lib/
export PATH=/usr/local/siox/bin:$PATH

echo "Running daemon"
rm *.dat /tmp/daemon.socket 2>/dev/null || true
killall siox-daemon || true

siox-daemon --configEntry=daemon.conf &

echo "Compiling"
gcc -g -Wall fwrite.c -o fwrite
mpicc.mpich2 test.c -Wall -g -o test

echo "Running the APP with SIOX instrumentation to create ontology etc."
siox-inst posix ./fwrite
siox-inst mpi ./test

echo "Running the APP with SIOX instrumentation"
mpiexec.mpich2 -np 4 siox-inst posix ./test

echo "Run completed!"


echo 
echo "Trace output"

siox-trace-reader


killall siox-daemon || true
