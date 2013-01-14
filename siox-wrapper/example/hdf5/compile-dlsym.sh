#!/bin/bash

gcc -c -fPIC hdf5-dlsym.c `pkg-config --cflags glib-2.0`
gcc hdf5-dlsym.o -shared -ldl -lhdf5 -o hdf5.so `pkg-config --libs glib-2.0`
gcc filter-test.c -lhdf5 -o filter-test
