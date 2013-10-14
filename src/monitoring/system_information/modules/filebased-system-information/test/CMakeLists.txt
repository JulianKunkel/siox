project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	filebasedsysteminformation-test
	filebasedsysteminformation-test.cpp
)

target_link_libraries(
	filebasedsysteminformation-test
	siox-core-ModuleLoader 
	siox-monitoring-FileBasedSystemInformation
)

add_test(File-Based-System-Information filebasedsysteminformation-test)

