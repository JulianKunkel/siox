#!/bin/bash

gcc -c hdf5-wrap.c `pkg-config --cflags glib-2.0`
gcc -c filter-test.c `pkg-config --libs glib-2.0`
gcc filter-test.o hdf5-wrap.o -Wl,--wrap="H5Fcreate" -lhdf5 -o filter-test-wrap
