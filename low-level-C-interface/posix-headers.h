//interface POSIX_IO

//ontology ow "BYTES WRITTEN" 64_BIT_INTEGER
//ontology or "BYTES READ" 64_BIT_INTEGER
//ontology oo "OFFSET" 64_BIT_INTEGER


//activity
//cause pathname
//descriptorMap filename filehandle pathname function_return_value
       int open(const char *pathname, int flags, mode_t mode);

//activity
//cause pathname
//descriptorMap filename filehandle pathname function_return_value
       int creat(const char *pathname, mode_t mode);

//activity 
//cause fd
//releaseDescriptor filehandle fd
       int close(int fd);

//activity
//cause fd
//report ow function_return_value
//report oo lseek(fd, 0, SEEK_CUR)
       ssize_t write(int fd, const void *buf, size_t count);

//activity
//cause fd
//report or function_return_value
//report oo lseek(fd, 0, SEEK_CUR)
       ssize_t read(int fd, void *buf, size_t count);

//activity
//cause fd
//report or function_return_value
//report oo offset
       ssize_t pread(int fd, void *buf, size_t count, off_t offset);

//activity
//cause fd
//report ow function_return_value
//report oo offset
       ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
       off_t lseek(int fd, off_t offset, int whence);

//activity
//cause path
//descriptorMap filename filepointer path function_return_value
       FILE * fopen(const char *path, const char *mode);

//activity
//cause fd
//descriptorMap filehandle filepointer fd function_return_value
       FILE * fdopen(int fd, const char *mode);

//activity
//cause path
//descriptorMap filename filepointer path function_return_value
//releaseDescriptor filepointer stream
       FILE * freopen(const char *path, const char *mode, FILE * stream);

//activity
//cause fp
//releaseDescriptor filepointer fp
       int fclose(FILE *fp);

/// VORSICHT HIER bei REPORT
//activity
//cause stream
//report ow function_return_value*size
//report oo ftello(stream)
       size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

//activity
//cause stream
//report or function_return_value*size
//report oo ftello(stream)
       size_t fread(void *ptr, size_t size, size_t nmemb, FILE * stream);

       int fseeko(FILE *stream, off_t offset, int whence);
       int fseek(FILE *stream, long offset, int whence);
       int fsetpos(FILE *stream, fpos_t *pos);


//activity
//cause filename
//descriptorMap filename MPI_filehandle filename fh
MPI_File_open(MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh);

//activity
//cause mpi_fh
//splice int sSize; int retMPI = MPI_Type_size(datatype, & size);
//report ow sSize*count
//splice MPI_Offset m_offset; int retMPI = MPI_File_get_position(mpi_fh, & m_offset);
//warn retMPI != MPI_SUCCESS;
//report m_offset
int MPI_File_write(MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);

