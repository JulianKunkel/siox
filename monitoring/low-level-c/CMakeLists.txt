cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	siox-ll
	siox-ll.cpp
)

target_link_libraries(
	siox-ll
	AutoConfigurator
	module-loader
)

add_subdirectory(examples)
add_subdirectory(test)

