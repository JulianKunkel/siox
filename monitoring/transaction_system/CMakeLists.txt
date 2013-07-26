cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lpq -lrt")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/core/logger"
	"${CMAKE_SOURCE_DIR}/include/"
	"${CMAKE_SOURCE_DIR}/include/monitoring/datatypes"
	"${CMAKE_SOURCE_DIR}/monitoring/datatypes"
	"${PROTOBUF_INCLUDE_DIRS}"
	"${PostgreSQL_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
)

add_library(sioxts
	TransactionBackend.hpp
	PostgreSQLBackend.hpp
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.hpp
	PostgreSQLPumpCallback.cpp
	TransactionService.hpp
	TransactionService.cpp
 	${CMAKE_SOURCE_DIR}/include/monitoring/datatypes/Activity.hpp
# 	${CMAKE_SOURCE_DIR}/monitoring/datatypes/Activity.cpp
# 	${CMAKE_SOURCE_DIR}/include/monitoring/datatypes/Component.h
# 	${CMAKE_SOURCE_DIR}/monitoring/datatypes/Component.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(sioxts sioxcomm sioxlog ${LINK_LIBRARIES} ${PROTOBUF_LIBRARIES} ${PostgreSQL_LIBRARIES} ${Boost_LIBRARIES})

add_executable(tsd
	sioxtsd.cpp
)

target_link_libraries(tsd sioxts ${Boost_LIBRARIES})

add_executable(testclient
	testclient.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(testclient sioxcomm sioxlog ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
