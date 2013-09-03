project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

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
