#!/bin/bash -e

# create a library implementation which invokes printf for each call
# this is just an example, we will apply the same approach for SIOX-Low-Level

echo "Create wrapper with LD_PRELOAD"

python ../../siox-wrapper.py -o posix-printf-dlsym.c -t ./template-list.py -s dlsym low-level-io.h 

# create siox-test-library for LD_PRELOAD
gcc -shared siox-wrapper-test.c `pkg-config --cflags --libs glib-2.0` -fPIC -o libsiox-wrapper-test.so


# create library implementation
echo 'Compiling posix-printf.c as shared object posix.so'
gcc posix-printf-dlsym.c -ldl `pkg-config --cflags --libs glib-2.0` -shared -fPIC -o libposix-printf-dlysm.so


# link library to POSIX application for testing
echo 'Compiling test.c as test'
gcc test.c --std=c99 -o test-dlsym -L. -l siox-wrapper-test `pkg-config --cflags --libs glib-2.0`

gcc test.c --std=c99 -o test-dlsym-linked -L. `pkg-config --cflags --libs glib-2.0` -lposix-printf-dlysm

printf "\n---\n\n"
echo 'Executing test_posix with preloaded posix-printf.so'
export LD_LIBRARY_PATH=.
LD_PRELOAD=./libposix-printf-dlysm.so ./test-dlsym
./test-dlsym-linked

