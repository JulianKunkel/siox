cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${CMAKE_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/build/core/comm"

)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11")

add_library(sioxlog
	BasicLogger.h
	BasicLogger.cpp
	Logger.h
	Logger.cpp
	FileLogger.h
	FileLogger.cpp
	SyslogLogger.h
	SyslogLogger.cpp
	SioxLogger.h
	NetLogger.h
	NetLogger.cpp
	NetLoggerServer.h
	NetLoggerServer.cpp
)

target_link_libraries(sioxlog sioxcomm ${LINK_LIBRARIES})
