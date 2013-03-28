#!/bin/bash

echo 'Generating low-level-io-dlsym.c'
python ../../siox-wrapper.py -t ./template.py -o low-level-io-dlsym.c low-level-io.h

echo 'Compiling low-level-io-dlsym.c as shared object posix.so'
gcc low-level-io-dlsym.c -c -fPIC
gcc low-level-io-dlsym.o -o posix.so -shared -ldl

echo 'Compiling test.c as test_dlsym'
gcc test.c -o test_dlsym
printf "\n---\n\n"
echo 'Executing test_posix with preloaded posix.so'
LD_PRELOAD=./posix.so ./test_dlsym

printf '\n---\n\n'
echo -n 'Cat tmp_posix_test: '
cat tmp_posix_test
