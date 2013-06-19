#!/bin/bash -e

# create a library implementation which invokes printf for each call
# this is just an example, we will apply the same approach for SIOX-Low-Level

# create wrapper
echo 'Generating posix-printf.c'
python ../../siox-wrapper.py -o posix-printf-wrap.c -t ./template-list.py -s wrap low-level-io.h > options

echo 'Compiling posix-printf.c as shared object posix.so'
eval "gcc posix-printf-wrap.c -ldl `pkg-config --cflags --libs glib-2.0` -shared -fPIC -o libposix-printf-wrap.so $(cat options)"


echo 'Compiling test.c as test for wrap'
eval "gcc --std=c99 test.c -o test-wrap -L. -l posix-printf-wrap `pkg-config --cflags --libs glib-2.0` $(cat options)"

./test-wrap



