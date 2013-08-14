cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${SIOX_INCLUDE_DIR}/../monitoring"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_library(
	StatisticsMultiplexerSync SHARED
	StatisticsMultiplexerSync.cpp
)

target_link_libraries(
	StatisticsMultiplexerSync
	core-container-serializer
	"${LINK_LIBRARIES}"
)

add_subdirectory(test)
