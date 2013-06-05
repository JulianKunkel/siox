cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/core/logger"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/core/comm/misc"
	"${CMAKE_SOURCE_DIR}/core/comm/buffers"
	"${CMAKE_SOURCE_DIR}/core/comm/ipc"
	"${CMAKE_SOURCE_DIR}/core/comm/tcp"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_library(sioxcomm
	misc/URI.h
	misc/URI.cc
	ConnectionMessage.h
	ConnectionMessage.cc
	Service.h
	Callback.h
	Connection.h
	Connection.cc
	ConnectionFactory.h
	ConnectionFactory.cc
	ServerFactory.h
	ServerFactory.cc
	ServiceServer.h
	ServiceServer.cc
	ipc/IPCConnection.h
	ipc/IPCConnection.cc
	ipc/IPCServer.h
	ipc/IPCServer.cc
	tcp/TCPConnection.h
	tcp/TCPConnection.cc
	tcp/TCPServer.h
	tcp/TCPServer.cc
	ServiceClient.h
	ServiceClient.cc
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(sioxcomm sioxlog ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

