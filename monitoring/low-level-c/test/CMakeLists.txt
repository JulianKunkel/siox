project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	siox-ll-c-test
	siox-ll-c-test.c
)

target_link_libraries(
	siox-ll-c-test
	siox-monitoring-siox-ll 
	siox-monitoring-ActivityBuilder
)

add_test(SIOX-Low-Level-C siox-ll-c-test)
