cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(GLIB2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmodule-2.0")

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${GLIB2_INCLUDE_DIRS}"
)

add_library(
	module-loader
	module-loader.cpp
)

add_subdirectory(test)
