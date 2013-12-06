project(SIOX)

find_package(MPI)
set_package_properties(MPI PROPERTIES URL "http://www.open-mpi.org" TYPE OPTIONAL PURPOSE "SIOX Open MPI examples.")

if (MPI_FOUND)

# YES, this module should not belong here because it depends on MPI... 
# Will be moved to out-of-tree extra modules


include_directories(
	${SIOX_INCLUDE_DIRS}
	${MPI_CXX_INCLUDE_PATH}
)

add_library(
	siox-core-reporting-MPIReporter
	MPIReporter.cpp
	MPIReporterOptions.cpp
)

target_link_libraries(
	siox-core-reporting-MPIReporter
	siox-core-AutoConfigurator
	siox-core-reporting-ConsoleReporter
	${MPI_CXX_LIBRARIES}
)

siox_run_serializer(MPIReporterOptions.hpp MPIReporterOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-core-reporting-MPIReporter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

endif (MPI_FOUND)
