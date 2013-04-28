#!/bin/bash

# create a library implementation which invokes printf for each call
# this is just an example, we will apply the same approach for SIOX-Low-Level

# create wrapper
echo 'Generating posix-printf.c'
python ../../siox-wrapper.py -o posix-printf.c -t template-plain.py -s plain low-level-io.h

# create library implementation
echo 'Compiling posix-printf.c as shared object posix.so'
gcc posix-printf.c -shared -fPIC -o posix-printf.so

# link library to POSIX application for testing
echo 'Compiling test.c as test'
gcc ../simple-posix/test.c -o test

printf "\n---\n\n"
echo 'Executing test_posix with preloaded posix-printf.so'
LD_PRELOAD=./posix-printf.so ./test

