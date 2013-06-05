cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
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
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(sioxlog sioxcomm ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
