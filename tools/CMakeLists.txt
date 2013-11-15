project(SIOX)

find_package(NetCDF QUIET)
set_package_properties(NetCDF PROPERTIES URL "http://www.unidata.ucar.edu/software/netcdf" TYPE OPTIONAL PURPOSE "SIOX NetCDF examples.")

find_package(HDF5)
set_package_properties(HDF5 PROPERTIES URL "http://www.hdfgroup.org/HDF5" TYPE OPTIONAL PURPOSE "SIOX HDF5 examples.")

find_package(MPI)
set_package_properties(MPI PROPERTIES URL "http://www.open-mpi.org" TYPE OPTIONAL PURPOSE "SIOX Open MPI examples.")

add_subdirectory(siox-skeleton-builder)
