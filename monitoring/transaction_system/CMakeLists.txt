cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE}/core/buffers/siox.proto")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lpq -lrt")

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${PostgreSQL_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)

add_library(
	monitoring-transactionsystem SHARED
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.cpp
	TransactionService.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(
	monitoring-transactionsystem 
	${PROTOBUF_LIBRARIES} 
	${PostgreSQL_LIBRARIES} 
	${Boost_LIBRARIES}
)

