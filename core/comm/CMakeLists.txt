cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE}/core/buffers/siox.proto")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

add_library(
	core-communication
	ConnectionMessage.cpp
	Connection.cpp
	ConnectionFactory.cpp
	ServerFactory.cpp
	ServiceServer.cpp
	ipc/IPCConnection.cpp
	ipc/IPCServer.cpp
	tcp/TCPConnection.cpp
	tcp/TCPServer.cpp
	ServiceClient.cpp
	"${PROTO_SRCS}"
	"${PROTO_HDRS}"
)

target_link_libraries(
	core-communication 
	core-logger 
	"${PROTOBUF_LIBRARIES}"
	"${Boost_LIBRARIES}"
)
