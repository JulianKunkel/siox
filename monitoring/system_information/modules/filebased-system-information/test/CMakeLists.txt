cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_executable(
	filebasedsysteminformation-test
	filebasedsysteminformation-test.cpp
)

target_link_libraries(
	filebasedsysteminformation-test
	FileBasedSystemInformation
)
