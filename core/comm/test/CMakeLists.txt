project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-comm-mmts-in-memory
	test-mmts-interfaces.cpp
)

target_link_libraries(
	test-comm-mmts-in-memory
	siox-core-comm-multiMessageService 
	siox-core-comm-inmemory 
	siox-core-ModuleLoader
)


add_test(comm-mmts-in-memory test-comm-mmts-in-memory)
