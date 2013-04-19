cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(
	Protobuf REQUIRED
	Boost COMPONENTS thread system REQUIRED
)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS buffers/siox.proto)

include_directories(
	"${CMAKE_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/misc"
	"${PROJECT_SOURCE_DIR}/buffers"
	"${PROJECT_SOURCE_DIR}/ipc"
	"${PROJECT_SOURCE_DIR}/tcp"
	"${PROJECT_SOURCE_DIR}/test"
	"${PROTOBUF_INCLUDE_DIRS}"
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -pthread")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -pthread -lrt -lboost_system -lboost_thread")

add_library(siox
	misc/URI.h
	misc/URI.cc
	ConnectionMessage.h
	ConnectionMessage.cc
	Callback.h
	Callback.cc
	Connection.h
	Connection.cc
	ConnectionFactory.h
	ConnectionFactory.cc
	Server.h
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
	Service.h
	Service.cc
	ServiceClient.h
	ServiceClient.cc
	${PROTO_SRCS}
	${PROTO_HDRS}
)

add_executable(server 
	test/server.cc
)

add_executable(tcp_client 
	test/tcp_client.cc)

add_executable(ipc_service
	test/ipc_service.cc)

target_link_libraries(siox ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(server siox)
target_link_libraries(tcp_client siox)
target_link_libraries(ipc_service siox)
