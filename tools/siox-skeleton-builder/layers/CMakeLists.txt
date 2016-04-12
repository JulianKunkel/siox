project(SIOX)

add_subdirectory(posix)
add_subdirectory(posix-deedless)
#add_subdirectory(posix-replay)

if (MPI_FOUND)
	add_subdirectory(mpi)
	if (HDF5_FOUND)
		add_subdirectory(hdf5)
	endif (HDF5_FOUND)
	if (NetCDF_FOUND)
		add_subdirectory(netcdf4)
	endif (NetCDF_FOUND)
endif (MPI_FOUND)


