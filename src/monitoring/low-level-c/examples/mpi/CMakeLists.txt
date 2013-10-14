project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable (
	mpi-example
	mpi-example.c
)

