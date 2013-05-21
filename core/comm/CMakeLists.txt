cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)


PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS buffers/siox.proto)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/logger"
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/misc"
	"${PROJECT_SOURCE_DIR}/buffers"
	"${PROJECT_SOURCE_DIR}/ipc"
	"${PROJECT_SOURCE_DIR}/tcp"
	"${PROTOBUF_INCLUDE_DIRS}"
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lboost_system -lboost_thread")

add_library(siox
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

target_link_libraries(siox sioxlog ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

