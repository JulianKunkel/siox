project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable (
	hdf5-example
	hdf5-example.c
)
