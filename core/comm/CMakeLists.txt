cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(
	Protobuf REQUIRED
	Boost COMPONENTS system test thread REQUIRED
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

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lboost_system -lboost_thread -lboost_unit_test_framework")

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
	Service.h
	Service.cc
	ServiceClient.h
	ServiceClient.cc
	${PROTO_SRCS}
	${PROTO_HDRS}
)

add_executable(communication_tests
	test/communication.cc)

target_link_libraries(siox ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(communication_tests siox)

enable_testing()
add_test("Communication" communication_tests)
