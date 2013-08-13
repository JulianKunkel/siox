cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../include")

find_package(Boost 1.53.0 COMPONENTS serialization)
 
include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_executable(
	test-serializer
	test-serializer.cpp
)

target_link_libraries(
	test-serializer 
	core-container-serializer
	"${Boost_LIBRARIES}"
	"${LINK_LIBRARIES}"
)
