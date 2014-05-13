#!/bin/bash -e

echo 'Generating low-level-io-wrap.c'
python ../../siox-skeleton-builder.py -s wrap -t ../../template.py -o low-level-io-wrap.c low-level-io.h

echo 'Compiling test.c as test_wrap'
gcc low-level-io-wrap.c -c $(pkg-config glib-2.0 --libs --cflags)
gcc test.c -c
# Appending the output of the wrapper to compile the program
gcc test.o low-level-io-wrap.o -o test_wrap -Wl,--wrap="open",--wrap="close",--wrap="write"

printf '\n---\n\n'
echo 'Executing test_wrap'
./test_wrap

printf '\n---\n\n'
echo -n 'Cat tmp_posix_test: '
cat tmp_posix_test
