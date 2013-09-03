project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
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
