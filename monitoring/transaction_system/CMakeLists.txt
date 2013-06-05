cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lpq -lrt -lboost_system -lboost_thread")

include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/core/logger"
	"${CMAKE_SOURCE_DIR}/build/core/comm"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${PostgreSQL_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)


add_library(sioxts
	TransactionBackend.h
	PostgreSQLBackend.h
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.h
	PostgreSQLPumpCallback.cpp
	TransactionService.h
	TransactionService.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(sioxts sioxcomm sioxlog ${LINK_LIBRARIES} ${PostgreSQL_LIBRARIES} ${Boost_LIBRARIES})

add_executable(tsd
	sioxtsd.cpp
)

target_link_libraries(tsd sioxts  ${Boost_LIBRARIES})

add_executable(testclient
	testclient.cpp
)

target_link_libraries(testclient sioxcomm sioxlog)
