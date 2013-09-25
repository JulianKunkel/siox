project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	tools-TraceReader-test
	TraceReaderTest.cpp
)

target_link_libraries(
	tools-TraceReader-test
	siox-tools-TraceReader
)

symlink(activities.dat activities.dat)
symlink(association.dat association.dat)
symlink(ontology.dat ontology.dat)
symlink(system-info.dat system-info.dat)

add_test(tools-TraceReader-test tools-TraceReader-test)
