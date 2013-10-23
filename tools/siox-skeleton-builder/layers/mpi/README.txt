Additional requirements to build the MPI skeleton wrapper:
==========================================================
For Ubuntu:
	OpenMPI (only wrap methods works at the moment)
	$ sudo apt-get install libopenmpi1.6-dev libopenmpi1.6
	$ ./waf configure --siox=/usr/local/siox --prefix=/usr/local/siox --mpi=/usr/lib/openmpi/ --mpicc=/usr/bin/mpicc.openmpi
	MPICH2
	$ sudo apt-get install libmpich2-3 libmpich2-dev
	$ ./waf configure --siox=/usr/local/siox --prefix=/usr/local/siox --mpi=/usr/lib/mpich2 --mpicc=/usr/bin/mpicc.mpich2 

	Testing of LD_PRELOAD:
	$ cd test/
	$ mpicc.mpich2 -ggdb -Wall test.c 
	$ mpiexec.mpich2 -np 4  /usr/local/siox/bin/siox-inst mpi ./a.out 


