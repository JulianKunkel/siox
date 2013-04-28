#!/bin/bash
python ../../siox-wrapper.py -o siox-ll-dummy-implementation.c -t ../../template.py -s plain ../../../low-level-C-interface/siox-ll.h

# SIOX Fake-Implementation of low-level interface
gcc siox-ll-dummy-implementation.c -I  ../../../low-level-C-interface/ -shared -fPIC -o libsiox-ll-dummy-implementation.so `pkg-config --cflags --libs glib-2.0`
 

FLAGS=$(cat ../../example/posix/wrap-options.txt)

# Create simple test program
eval -- gcc -o test  -Wall ../simple-posix/test.c $FLAGS -L. -L../../example/posix/  -l siox-posix-ll-wrapper -l siox-ll-dummy-implementation


echo "I will run now test"
export LD_LIBRARY_PATH=.:../../example/posix/
./test



echo "Do the dlsym test"
gcc -o not-instrumented ../simple-posix/test.c
LD_PRELOAD=../../example/posix/libsiox-posix-ll-wrapper-dlsym.so:../low-level-dummy-implementierung/libsiox-ll-dummy-implementation.so ./not-instrumented
