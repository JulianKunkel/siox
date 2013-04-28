#!/bin/bash -e

OUTPUT_FILE=posix-ll-wrapper.c

python ../../siox-wrapper.py -s wrap -t ../../template.py -o $OUTPUT_FILE posix-low-level-io.h > wrap-options.txt

gcc $OUTPUT_FILE -c -fPIC -I ../../../low-level-C-interface `pkg-config --cflags glib-2.0`
gcc ${OUTPUT_FILE%%.c}.o -o libsiox-posix-ll-wrapper.so -shared -ldl `pkg-config --libs glib-2.0`

