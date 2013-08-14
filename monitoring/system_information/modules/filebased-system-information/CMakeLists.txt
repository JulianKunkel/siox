cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")


add_library(
	FileBasedSystemInformation SHARED
	FileBasedSystemInformation.cpp
)

target_link_libraries(
	FileBasedSystemInformation
	module-loader
	"${LINK_LIBRARIES}"
	"${Boost_LIBRARIES}"
)

add_subdirectory(test)

