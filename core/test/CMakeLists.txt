find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${CMAKE_SOURCE_DIR}/core/comm/buffers/siox.proto")

include_directories(
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/core/comm"
	"${CMAKE_SOURCE_DIR}/build/core/comm"
	"${CMAKE_SOURCE_DIR}/core/logger"
	"${CMAKE_SOURCE_DIR}/core/config"
	"${PROTOBUF_INCLUDE_DIRS}"
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lboost_system -lboost_thread -lboost_unit_test_framework")

add_executable(communication_tests
	communication.cc
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(communication_tests sioxcomm)

add_executable(configuration_tests
	configuration.cc
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(configuration_tests sioxconf sioxlog)
