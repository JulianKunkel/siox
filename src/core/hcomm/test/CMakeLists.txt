project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-hcomm-mmts-in-memory
	test-mmts-interfaces.cpp
)

target_link_libraries(
	core-hcomm-mmts-in-memory
	siox-core-hcomm-multiMessageService 
	siox-core-comm-inmemory 
	siox-core-ModuleLoader
	siox-core-comm
)

add_test(core-hcomm-mmts-in-memory core-hcomm-mmts-in-memory)


add_executable(
	core-hcomm-high-level-test
	high-level-client-test.cpp
)
target_link_libraries(
	core-hcomm-high-level-test
	siox-core-ModuleLoader
	siox-core-comm-gio
	siox-core-hcomm
)

 add_test(core-hcomm-high-level-test core-hcomm-high-level-test)
