project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
)

add_executable(
	converter-test
	converter-test.cpp
)

target_link_libraries(
	converter-test
	core-buffer-converter
	core-comm-buffers
)

add_test(SIOX-Protocol-Buffer-Converter converter-test)
