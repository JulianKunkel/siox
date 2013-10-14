project(SIOX)

if (HDF5_FOUND)
	add_subdirectory(hdf5)
endif (HDF5_FOUND)

if (NetCDF_FOUND)
	add_subdirectory(netcdf)
endif (NetCDF_FOUND)

if (MPI_FOUND)
	#add_subdirectory(mpi)
endif (MPI_FOUND)

add_subdirectory(mock-up-fs)
