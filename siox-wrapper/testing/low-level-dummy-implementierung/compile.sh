#!/bin/bash
python ../../siox-wrapper.py -o siox-ll-dummy-implementation.c -t ../../template.py -s plain ../../../low-level-C-interface/siox-ll.h

# SIOX Fake-Implementation of low-level interface
gcc siox-ll-dummy-implementation.c -shared -fPIC -o libsiox-ll-dummy-implementation.so

# Create simple test program
gcc -o test  -Wall ../simple-posix/test.c -L. -L../../example/posix/  -l siox-posix-ll-wrapper -l siox-ll-dummy-implementation
