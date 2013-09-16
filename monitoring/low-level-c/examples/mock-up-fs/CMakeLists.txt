project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	test-mufs
	mufs.c
)

target_link_libraries(
	test-mufs
	siox-monitoring-siox-ll
)

add_executable(
	mockupfs-example
	mockupfs-example.c
)

target_link_libraries(
	mockupfs-example
	test-mufs 
	siox-monitoring-siox-ll 
	siox-monitoring-ActivityBuilder
)

