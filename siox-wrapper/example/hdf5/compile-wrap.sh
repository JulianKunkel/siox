#!/bin/bash

gcc -c hdf5-wrap.c
gcc -c filter-test.c
gcc filter-test.o hdf5-wrap.o -Wl,--wrap="H5Fcreate" -lhdf5 -o filter-test-wrap
