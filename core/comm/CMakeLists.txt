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
	misc/URI.hpp
	misc/URI.cpp
	ConnectionMessage.hpp
	ConnectionMessage.cpp
	Service.hpp
	Callback.hpp
	Connection.hpp
	Connection.cpp
	ConnectionFactory.hpp
	ConnectionFactory.cpp
	ServerFactory.hpp
	ServerFactory.cpp
	ServiceServer.hpp
	ServiceServer.cpp
	ipc/IPCConnection.hpp
	ipc/IPCConnection.cpp
	ipc/IPCServer.hpp
	ipc/IPCServer.cpp
	tcp/TCPConnection.hpp
	tcp/TCPConnection.cpp
	tcp/TCPServer.hpp
	tcp/TCPServer.cpp
	ServiceClient.hpp
	ServiceClient.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(sioxcomm sioxlog ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

