cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE_DIR}/core/buffers/siox.proto")

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lpq -lrt")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${SIOX_INCLUDE_DIR}/core/comm"
	"${SIOX_INCLUDE_DIR}/core/logger"
	"${SIOX_INCLUDE_DIR}/misc"
	"${SIOX_INCLUDE_DIR}/monitoring"
	"${SIOX_INCLUDE_DIR}/monitoring/datatypes"
	"${SIOX_INCLUDE_DIR}/monitoring/transaction_system"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${PostgreSQL_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)

add_library(monitoring-transactionsystem
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.cpp
	TransactionService.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(monitoring-transactionsystem ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${PostgreSQL_LIBRARIES} ${Boost_LIBRARIES})

