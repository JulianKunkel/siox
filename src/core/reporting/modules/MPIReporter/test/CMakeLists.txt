project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-reporting-MPIReporter
	test.cpp
)

target_link_libraries(
	core-reporting-MPIReporter
	siox-core-ModuleLoader
	siox-core-reporting-MPIReporter
)

add_test(core-reporting-MPIReporter core-reporting-MPIReporter)
