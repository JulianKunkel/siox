cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

include(FindProtobuf)

find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${SIOX_INCLUDE_DIR}/core/buffers/siox.proto")

# include_directories(
# 	"${CMAKE_CURRENT_BINARY_DIR}"
# 	"${SIOX_INCLUDE_DIR}/core/comm"
# 	"${SIOX_INCLUDE_DIR}/core/logger"
# 	"${SIOX_INCLUDE_DIR}/misc/"
# 	"${PROTOBUF_INCLUDE_DIRS}"
# 	"${Boost_INCLUDE_DIR}"
# )
# 
# set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")
# 
# add_executable(core-communication-tests
# 	communication.cpp
# 	${PROTO_SRCS}
# 	${PROTO_HDRS}
# )
# 
# target_link_libraries(core-communication-tests core-communication ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})
