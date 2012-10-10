#!/bin/bash

gcc low-level-io-dlsym.c -c -fPIC -I ../../../low-level-C-interface
gcc low-level-io-dlsym.o -o posix.so /home/yoo/docs/uni/dkrz/sioxhh/Komponenten/low-level-C-interface/libsiox-ll.so -shared -ldl
