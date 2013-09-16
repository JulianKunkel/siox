project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-serializer
	test-serializer.cpp
)

target_link_libraries(
	test-serializer 
	siox-core-ContainerSerializer
)

add_test(Serializer test-serializer)
