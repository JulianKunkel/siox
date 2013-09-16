project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
)

add_library(
	core-buffer-converter
	BufferConverter.cpp
)

target_link_libraries(
	core-buffer-converter
	core-comm-buffers
)

add_subdirectory(test)
