cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SIOX_INCLUDE_DIR}/cmake/cmake-glib2")

find_package(GLIB2 REQUIRED)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lgmodule-2.0")


add_executable(test-module-loader
	main.cpp
)

target_link_libraries(
	test-module-loader
	test-module
	${LINK_LIBRARIES}
	${GLIB2_LIBRARIES}
)

add_library(test-module SHARED
	test-module.cpp
)

target_link_libraries(
	test-module
	core-module
	${LINK_LIBRARIES}
	${GLIB2_LIBRARIES}
)

