#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1048576 //1024KB: 1024*1024
void main(int argc, char *argv[]) {

	// for DEBUG
	int gdb = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
    while (0 == gdb)
        sleep(5);

	// various variables
	int my_rank, my_size, i;
	int number_of_blocks = 64; // 1 block = 4 MB
	int master_rank = FALSE;
	char file_name[30];
	int filename_length, number_of_integers, number_of_bytes;
	int *junk;
	long long total_number_of_bytes;
	MPI_Offset my_offset, my_current_offset;
	MPI_File fh;
	MPI_Status status;
	double start_time, finish_time, io_time, longest_io_time;
		
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &my_size);
	if (my_rank == MASTER_RANK)
		master_rank = TRUE;

	//set the file name
	snprintf(file_name, 30, "/siox-gpfs/with_gpfs_hints_%d", 0);
	printf("Creating file %s\n", file_name);

	filename_length = strlen(file_name) + 1;

	MPI_Bcast(&number_of_blocks, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
	MPI_Bcast(&filename_length, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
	MPI_Bcast(file_name, filename_length, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);

	number_of_integers = number_of_blocks * BLOCK_SIZE;
	number_of_bytes = sizeof(int) * number_of_integers;

	total_number_of_bytes = (long long) my_size * (long long) number_of_bytes;
	my_offset = (long long) my_rank * (long long) number_of_bytes;

	if (master_rank) {
		printf("number_of_bytes = %d/\n", number_of_bytes);
		printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
		printf("size of offset = %d bytes\n", sizeof(MPI_Offset));
	}

	MPI_Info siox_info;
	char siox_access_range_value[50];//"START, LENGTH, READ/WRITE"
	char siox_access_range_start[25], siox_access_range_length[25];
	MPI_Info_create(&siox_info);
	MPI_Info_set(siox_info, "useSIOXLib", "false");

	snprintf(siox_access_range_start, 25, "%lld,", my_offset);
	snprintf(siox_access_range_length, 25, "%lld,", number_of_bytes);
	strcpy(siox_access_range_value, siox_access_range_start);
	strcat(siox_access_range_value, siox_access_range_length);
	strcat(siox_access_range_value, "1");
	MPI_Info_set(siox_info, "sioxAccessRange", siox_access_range_value);

	//MPI_Info_set(siox_info, "sioxClearFileCache", "true");
	//MPI_Info_set(siox_info, "sioxCancelHints", "false");

	MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDWR | MPI_MODE_CREATE,
			siox_info, &fh);
	MPI_File_seek(fh, my_offset, MPI_SEEK_SET);
	MPI_File_get_position(fh, &my_current_offset);
	printf("Rank %d: the current offset is %lld\n", my_rank, my_current_offset);

	junk = (int*) malloc(number_of_bytes);
	for (i = 0; i < number_of_integers; i++)
		*(junk + i) = rand();

	start_time = MPI_Wtime();
	MPI_File_write(fh, junk, number_of_integers, MPI_INT, &status);
	finish_time = MPI_Wtime();
	io_time = finish_time - start_time;
	MPI_File_close(&fh);
	MPI_Allreduce(&io_time, &longest_io_time, 1, MPI_DOUBLE, MPI_MAX,
			MPI_COMM_WORLD);

	if (master_rank) {
		printf("longest_io_time = %f seconds\n", longest_io_time);
		printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
		printf("transfer rate = %f MB/s\n", total_number_of_bytes
				/ longest_io_time / 1048576);
	}
	//Second Writing
	MPI_Info siox_info_1;
	MPI_Info_create(&siox_info_1);
	MPI_Info_set(siox_info_1, "useSIOXLib", "true");
	snprintf(siox_access_range_start, 25, "%lld,", my_offset);
	snprintf(siox_access_range_length, 25, "%lld,", number_of_bytes);
	strcpy(siox_access_range_value, siox_access_range_start);
	strcat(siox_access_range_value, siox_access_range_length);
	strcat(siox_access_range_value, "1");
	MPI_Info_set(siox_info_1, "sioxAccessRange", siox_access_range_value);
	MPI_Info_set(siox_info_1, "sioxClearFileCache", "true");
	MPI_Info_set(siox_info_1, "sioxCancelHints", "false");
	MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDWR | MPI_MODE_CREATE,
			siox_info_1, &fh);
	MPI_File_seek(fh, my_offset, MPI_SEEK_SET);
	MPI_File_get_position(fh, &my_current_offset);
	printf("Second writing: Rank %d: the current offset is %lld\n", my_rank, my_current_offset);
	junk = (int*) malloc(number_of_bytes);
	for (i = 0; i < number_of_integers; i++)
		*(junk + i) = rand();
	start_time = MPI_Wtime();
	MPI_File_write(fh, junk, number_of_integers, MPI_INT, &status);
	finish_time = MPI_Wtime();
	io_time = finish_time - start_time;
	MPI_File_close(&fh);
	MPI_Allreduce(&io_time, &longest_io_time, 1, MPI_DOUBLE, MPI_MAX,
			MPI_COMM_WORLD);
	if (master_rank) {
		printf("Second writing: longest_io_time = %f seconds\n", longest_io_time);
		printf("Second writing: total_number_of_bytes = %lld\n", total_number_of_bytes);
		printf("Second writing: transfer rate = %f MB/s\n", total_number_of_bytes
				/ longest_io_time / 1048576);
	}
	
	MPI_Finalize();

}
