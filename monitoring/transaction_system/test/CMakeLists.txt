cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/core/logger"
	"${CMAKE_SOURCE_DIR}/monitoring/transaction_system"
	"${CMAKE_SOURCE_DIR}/include/C"
	"${CMAKE_SOURCE_DIR}/include/misc"
	"${CMAKE_SOURCE_DIR}/include/monitoring/datatypes"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_executable(postgresql_test
	postgres.cpp
	"${CMAKE_SOURCE_DIR}/monitoring/transaction_system/PostgreSQLBackend.cpp"
	"${CMAKE_SOURCE_DIR}/monitoring/transaction_system/PostgreSQLPumpCallback.cpp"
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(postgresql_test sioxts sioxcomm ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

