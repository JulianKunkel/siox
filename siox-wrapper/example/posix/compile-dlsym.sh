#!/bin/bash

gcc low-level-io-dlsym.c -c -fPIC -I ../../../low-level-C-interface `pkg-config --cflags glib-2.0`
gcc low-level-io-dlsym.o -o posix.so ../../../low-level-C-interface/libsiox-ll.so -shared -ldl `pkg-config --libs glib-2.0`

