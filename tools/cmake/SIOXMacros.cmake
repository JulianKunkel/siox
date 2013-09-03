macro(SIOX_GENERATE_BUFFERS)
	PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SIOX_INCLUDE}/core/comm/messages/siox.proto)
	add_library(core-commm-buffers ${PROTO_SRCS} ${PROTO_HDRS})
endmacro(SIOX_GENERATE_BUFFERS)

