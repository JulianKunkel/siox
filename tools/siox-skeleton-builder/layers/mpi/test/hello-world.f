c  Trivial fortran example
      program MPI_simple_test
      include 'mpif.h'
      integer rank, size, tag, ierror, status(MPI_STATUS_SIZE)
      
      call MPI_INIT(ierror)
      call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierror)
      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

      print *, 'Hello from rank: ', rank
      call MPI_BARRIER(MPI_COMM_WORLD, ierror)

      call MPI_FINALIZE(ierror)

      end
