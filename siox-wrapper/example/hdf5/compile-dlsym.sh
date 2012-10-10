#!/bin/bash

gcc -c -fPIC hdf5-dlsym.c
gcc hdf5-dlsym.o -shared -ldl -lhdf5 -o hdf5.so
gcc filter-test.c -lhdf5 -o filter-test
