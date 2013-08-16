cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_executable(
	systeminfo_test
	systeminfo_test.cpp
)

target_link_libraries(
	systeminfo_test
	knowledge-systeminfo
)


