cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SIOX_INCLUDE_DIR}/cmake/cmake-glib2")

find_package(GLIB2 REQUIRED)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${SIOX_INCLUDE_DIR}/core/module"
	"${GLIB2_INCLUDE_DIRS}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -lgmodule-2.0")

add_library(
	module-loader SHARED
	module-loader.cpp
)

target_link_libraries(
	module-loader 
	"${GLIB2_LIBRARIES}"
	"${LINK_LIBRARIES}"
)

add_subdirectory(test)
