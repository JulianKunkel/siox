project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HDF5_DEFINITIONS}")

add_executable (
	hdf5-example
	hdf5-example.c
	${HDF5_INCLUDE_DIRS}

)


target_link_libraries(
	hdf5-example
	siox-monitoring-siox-ll 
	${HDF5_LIBRARIES}
	${HDF5_HL_LIBRARIES}
)
