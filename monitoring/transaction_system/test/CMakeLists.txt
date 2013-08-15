cmake_minimum_required(VERSION 2.8)
project(SIOX)


include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

list(REMOVE_ITEM PROTOBUF_LIBRARIES "debug" "optimized")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE}/core/buffers/siox.proto")

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

add_executable(postgresql_test
	postgres.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(postgresql_test 
	monitoring-transactionsystem 
	core-communication 
	core-logger
	${PROTOBUF_LIBRARIES} 
	${Boost_LIBRARIES}
)

