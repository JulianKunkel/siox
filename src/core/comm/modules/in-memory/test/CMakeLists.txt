project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-comm-in-memory
	test-comm-interfaces.cpp
)

target_link_libraries(
	test-comm-in-memory
	siox-core-ModuleLoader
	siox-core-comm-inmemory 
)

add_test(comm-in-memory test-comm-in-memory)
