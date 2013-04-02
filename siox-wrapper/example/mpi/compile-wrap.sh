#!/bin/bash -e

OUTPUT_FILE=mpi-wrapper.c

echo 'Generating' $OUTPUT_FILE

python ../../siox-wrapper.py -s wrap -t ../../template.py -o $OUTPUT_FILE mpi.h > wrap-options.txt

#gcc $OUTPUT_FILE -c -fPIC -I ../../../mpi-C-interface `pkg-config --cflags glib-2.0`
#gcc ${OUTPUT_FILE%%.c}.o -o libsiox-mpi-wrapper.so -shared -ldl `pkg-config --libs glib-2.0`
