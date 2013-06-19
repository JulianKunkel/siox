#!/bin/bash -e

OUTPUT_FILE=posix-ll-wrapper.c

python ../../siox-wrapper.py -s dlsym -t ../../template.py -o $OUTPUT_FILE posix-low-level-io.h 

gcc $OUTPUT_FILE -c -fPIC -I ../../../low-level-C-interface `pkg-config --cflags glib-2.0`
gcc ${OUTPUT_FILE%%.c}.o -o libsiox-posix-ll-wrapper-dlsym.so -shared -ldl `pkg-config --libs glib-2.0`

