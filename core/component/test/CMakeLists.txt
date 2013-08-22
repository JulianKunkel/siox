cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_executable(
	component-naming-test
	component-naming-test.cpp
)

target_link_libraries(
	component-naming-test
)

