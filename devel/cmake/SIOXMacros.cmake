macro(SIOX_RUN_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating boost serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
	ARGS -I ${CMAKE_SOURCE_DIR}/src/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_SERIALIZER)


macro(SIOX_RUN_BINARY_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating binary serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
	ARGS -s JBinary -I ${CMAKE_SOURCE_DIR}/src/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_BINARY_SERIALIZER)

 macro(SIOX_RUN_TEXT_SERIALIZER IN OUT)
    add_custom_command(
	OUTPUT ${OUT}
	COMMENT "Generating text serialization ${IN}"
	COMMAND ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
	ARGS -f text -I ${CMAKE_SOURCE_DIR}/src/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
    )
endmacro(SIOX_RUN_TEXT_SERIALIZER)

macro(SYMLINK IN OUT)

	string(REPLACE "/" "_" CLEANED_DIRNAME ${CMAKE_CURRENT_SOURCE_DIR})

	add_custom_target(symlink${CLEANED_DIRNAME}${IN} ALL
			COMMENT "Symlink ${CMAKE_CURRENT_SOURCE_DIR}/${IN}"
    		COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/${IN} ${IN}
	)
endmacro(SYMLINK)

 
