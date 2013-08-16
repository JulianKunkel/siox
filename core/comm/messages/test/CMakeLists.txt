cmake_minimum_required(VERSION 2.8)
project(SIOX)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SIOX_INCLUDE}/core/comm/messages/siox.proto)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

add_executable(
	converter-test
	converter-test.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(
	converter-test
	core-buffer-converter
	${PROTOBUF_LIBRARIES} 
	${Boost_LIBRARIES}
)
