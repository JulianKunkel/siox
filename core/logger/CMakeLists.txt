cmake_minimum_required(VERSION 2.8)
project(SIOX)


include_directories(
	"${CMAKE_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lboost_system -lboost_thread -lboost_unit_test_framework")

add_library(sioxlog
	Logger.h
	Logger.cpp
	FileLogger.h
	FileLogger.cpp
	SyslogLogger.h
	SyslogLogger.cpp
)


target_link_libraries(sioxlog ${LINK_LIBRARIES})
