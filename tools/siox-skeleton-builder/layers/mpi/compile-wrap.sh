#!/bin/bash -e

OUTPUT_FILE=mpi-wrapper.c

python ../../siox-skeleton-builder.py -s wrap -t ../../template.py -o $OUTPUT_FILE mpi-wrapper.h > wrap-options.txt

mpicc $OUTPUT_FILE -c -fPIC -std=c99 -I ../../../low-level-C-interface `pkg-config --cflags glib-2.0`
mpicc ${OUTPUT_FILE%%.c}.o -o libsiox-mpi-wrapper.so -shared -ldl `pkg-config --libs glib-2.0`