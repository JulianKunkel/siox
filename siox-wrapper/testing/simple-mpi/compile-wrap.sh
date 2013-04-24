#!/bin/bash -e

OUTPUT_FILE=mpi-wrapper.c

echo 'Generating' $OUTPUT_FILE

python ../../siox-wrapper.py -s wrap -t ./template.py -o $OUTPUT_FILE mpi-wrapper.h > wrap-options.txt

echo 'Compiling mpi-wrapper.c as mpi-wrapper'
gcc mpi-wrapper.c -c
gcc test.c -c
# Appending the output of the wrapper to compile the program
gcc test.o mpi-wrapper.o -o test_wrap -Wl,--wrap="MPI_Init",--wrap="MPI_File_open",--wrap="MPI_File_set_view",--wrap="MPI_File_read",--wrap="MPI_File_write",--wrap="MPI_File_close",--wrap="MPI_Finalize",--wrap="MPI_Comm_size",--wrap="MPI_Comm_rank"

printf '\n---\n\n'
echo 'Executing test_wrap'
./mpirun -np 4 test_wrap

#printf '\n---\n\n'
#echo -n 'Cat tmp_posix_test: '
#cat tmp_posix_test

#gcc $OUTPUT_FILE -c -fPIC -I ../../../mpi-C-interface `pkg-config --cflags glib-2.0`
#gcc ${OUTPUT_FILE%%.c}.o -o libsiox-mpi-wrapper.so -shared -ldl `pkg-config --libs glib-2.0`
