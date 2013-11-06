project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	MPIWrapperTest
	test.c
)

target_link_libraries(
	MPIWrapperTest
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityFileWriter
	${MPI_CXX_LIBRARIES}
)

add_test(tools-skeleton_builder-layers-mpi-mpi-test MPIWrapperTest)

