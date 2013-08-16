cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SIOX_INCLUDE}/core/buffers/siox.proto)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

add_executable(
	core-communication-tests
	communication.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(
	core-communication-tests 
	core-communication 
	${PROTOBUF_LIBRARIES} 
	${Boost_LIBRARIES}
)
