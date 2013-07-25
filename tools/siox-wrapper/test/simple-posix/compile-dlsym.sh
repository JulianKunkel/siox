#!/bin/bash -e

echo 'Generating low-level-io-dlsym.c'
python ../../siox-wrapper.py -t ../../template.py -o low-level-io-dlsym.c low-level-io.h

echo 'Compiling low-level-io-dlsym.c as shared object posix.so'
gcc low-level-io-dlsym.c -c -fPIC -I/usr/local/include $(pkg-config glib-2.0 --cflags)
gcc low-level-io-dlsym.o -o posix.so -shared -ldl $(pkg-config glib-2.0 --libs) -L/usr/local/lib -lsiox-ll
