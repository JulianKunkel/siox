macro(SIOX_GENERATE_BUFFERS)
	PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SIOX_INCLUDE}/core/comm/messages/siox.proto)
	add_library(core-comm-buffers ${PROTO_SRCS} ${PROTO_HDRS})
	target_link_libraries(core-comm-buffers ${PROTOBUF_LIBRARIES})
endmacro(SIOX_GENERATE_BUFFERS)

macro(SIOX_RUN_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating boost serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/tools/serialization-code-generator.py
	ARGS -I ${CMAKE_SOURCE_DIR}/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_SERIALIZER)


macro(SIOX_RUN_BINARY_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating binary serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/tools/serialization-code-generator.py
	ARGS -s JBinary -I ${CMAKE_SOURCE_DIR}/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_BINARY_SERIALIZER)

 macro(SIOX_RUN_TEXT_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating text serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/tools/serialization-code-generator.py
	ARGS -f text -I ${CMAKE_SOURCE_DIR}/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_TEXT_SERIALIZER)

macro(SYMLINK IN OUT)

	string(REPLACE "/" "_" CLEANED_DIRNAME ${CMAKE_CURRENT_SOURCE_DIR})

	add_custom_target(symlink${CLEANED_DIRNAME}${IN} ALL
			COMMENT "Symlink ${CMAKE_CURRENT_SOURCE_DIR}/${IN}"
    		COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/${IN} ${IN}
	)
endmacro(SYMLINK)

 
