cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../include")

find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE_DIR}/core/buffers/siox.proto")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${SIOX_INCLUDE_DIR}/C"
	"${SIOX_INCLUDE_DIR}/core/comm"
	"${SIOX_INCLUDE_DIR}/core/datatypes"
	"${SIOX_INCLUDE_DIR}/core/logger"
	"${SIOX_INCLUDE_DIR}/misc"
	"${SIOX_INCLUDE_DIR}/monitoring/datatypes"
	"${SIOX_INCLUDE_DIR}/monitoring/transaction_system"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_executable(postgresql_test
	postgres.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(postgresql_test 
	monitoring-transactionsystem 
	core-communication 
	${PROTOBUF_LIBRARIES} 
	${Boost_LIBRARIES}
)

