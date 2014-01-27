project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-comm-gio-test
	test-comm-interfaces.cpp
)

target_link_libraries(
	core-comm-gio-test
	siox-core-ModuleLoader
	siox-core-comm-gio 
)

add_executable(
	gio-performance-test
	performanceTest.cpp
)

target_link_libraries(
	gio-performance-test
	siox-core-ModuleLoader
	siox-core-comm-gio
	rt
)


add_test(core-comm-gio-test core-comm-gio-test)
