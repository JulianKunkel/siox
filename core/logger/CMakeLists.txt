cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${CMAKE_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11")

add_library(sioxlog
	Logger.h
	Logger.cpp
	FileLogger.h
	FileLogger.cpp
	SyslogLogger.h
	SyslogLogger.cpp
	SioxLogger.h
	SioxLogger.cpp
)

target_link_libraries(sioxlog ${LINK_LIBRARIES})
