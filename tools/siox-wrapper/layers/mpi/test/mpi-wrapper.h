

/* Set the interface name for the library*/
//@component "MPIIO"

/* Register the metrics to grab the performance data */

/* Prepare a (hash) map to link descriptors (in this case, of type int) to their activities.
   This is necessary for the horizontal linking of activities.
   The map and all other functions using it default to the same built-in name,
   so it can usually be omitted. */
//@horizontal_map_create_int
//@horizontal_map_create_str

/*------------------------------------------------------------------------------
End of global part
------------------------------------------------------------------------------*/
//@activity Name="MPI_Comm_size"
//@error ''ret!=0'' ret
int MPI_Comm_size(MPI_Comm comm, int *size);

//@activity Name="MPI_Comm_rank"
//@error ''ret!=0'' ret
int MPI_Comm_rank(MPI_Comm comm, int *rank);

//@activity Name="MPI_Init"
//@error ''ret!=0'' ret
int MPI_Init(int *argc, char ***argv);

//@activity_with_hints hints_name hints_value Name="MPI_File_open"
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_open(MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh);

//@activity_with_hints hints_name hints_value Name="MPI_File_set_view"
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info);

//@activity Name="MPI_File_read"
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);

//@activity Name="MPI_File_write"
//@activity_link_int fh
//@error ''ret!=0'' ret
int MPI_File_write(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);

//@activity Name="MPI_File_close"
//@horizontal_map_remove_int fh
//@error ''ret!=0'' ret
int MPI_File_close(MPI_File *fh);

//@activity Name="MPI_Finalize"
//@horizontal_map_remove_int fh
//@horizontal_map_remove_str filename
//@error ''ret!=0'' ret
int MPI_Finalize(void);

