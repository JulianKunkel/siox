cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(GLIB2 REQUIRED)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${GLIB2_INCLUDE_DIRS}"
)

add_executable(
	test-module-loader
	main.cpp
)

target_link_libraries(
	test-module-loader
	test-module
	"${GLIB2_LIBRARIES}"
)

add_library(test-module SHARED
	test-module.cpp
)

target_link_libraries(
	test-module
	module-loader
	"${GLIB2_LIBRARIES}"
)

