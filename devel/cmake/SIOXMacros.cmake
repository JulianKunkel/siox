macro(SIOX_RUN_SERIALIZER IN OUT)
	add_custom_command(
		OUTPUT ${OUT}
		COMMENT "Generating boost serialization ${IN}"
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
		ARGS -I ${CMAKE_SOURCE_DIR}/src/include ${CMAKE_SOURCE_DIR}/src -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
	)
endmacro(SIOX_RUN_SERIALIZER)


macro(SIOX_RUN_BINARY_SERIALIZER IN OUT)
	add_custom_command(
		OUTPUT ${OUT}
		COMMENT "Generating binary serialization ${IN}"
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
		ARGS -s JBinary -I ${CMAKE_SOURCE_DIR}/src/include -i ${CMAKE_CURRENT_SOURCE_DIR}/${IN} -o ${OUT} 
	)
endmacro(SIOX_RUN_BINARY_SERIALIZER)

macro(SIOX_RUN_TEXT_SERIALIZER IN OUT)
	add_custom_command(
		OUTPUT ${OUT}
		COMMENT "Generating text serialization ${IN}"
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/devel/scripts/serialization-code-generator.py
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

 
macro(SIOX_SKELETON_BUILDER TYPE INPUT TEMPLATE LAYER OUTPUT)
	message("-- Generating ${TYPE} skeleton for ${INPUT}.")
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/siox-skeleton-builder/siox-skeleton-builder.py -s ${TYPE} -t ${TEMPLATE} -W ${LAYER}.wrap -o ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT} ${INPUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
endmacro(SIOX_SKELETON_BUILDER)

macro(SET_LIBRARY_INSTALL_SUFFIX)
	get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
	if (${LIB64} STREQUAL "TRUE")
		set(LIBRARY_INSTALL_SUFFIX 64)
	else()
		set(LIBRARY_INSTALL_SUFFIX "")
	endif()
endmacro(SET_LIBRARY_INSTALL_SUFFIX)
