project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	test-serializer
	test-serializer.cpp
	test-serializable.cpp
)

target_link_libraries(
	test-serializer 
	siox-core-ContainerSerializer
)

add_executable(
	test-bin-serializer
	test-bin-serializer.cpp
	test-bin-serializerBinary.hpp
)


siox_run_serializer(test-serializer.hpp test-serializable.cpp)

siox_run_binary_serializer(test-bin-serializer.hpp test-bin-serializerBinary.hpp)

add_test(Serializer test-serializer)
add_test(SerializerBinary test-bin-serializer)
