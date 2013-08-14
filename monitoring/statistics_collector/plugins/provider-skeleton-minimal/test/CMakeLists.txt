cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../include")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_executable(
	iostatsTestSkel
	iostatsTest.cpp
)

target_link_libraries(
	iostatsTestSkel
	StatisticsProviderSkeleton
	AutoConfigurator
	module-loader
	"${LINK_LIBRARIES}"
)
