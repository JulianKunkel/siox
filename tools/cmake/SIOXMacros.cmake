macro(SIOX_GENERATE_BUFFERS)
	PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SIOX_INCLUDE}/core/comm/messages/siox.proto)
	add_library(core-comm-buffers ${PROTO_SRCS} ${PROTO_HDRS})
	target_link_libraries(core-comm-buffers ${PROTOBUF_LIBRARIES})
endmacro(SIOX_GENERATE_BUFFERS)

