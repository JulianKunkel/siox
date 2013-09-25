project(SIOX)

include_directories(${SIOX_INCLUDE_DIRS})

add_library(
	siox-core-ActivitySerializable
	ActivitySerializableText.cpp
)

add_library(
	siox-core-ComponentRegistrar
	ComponentRegistrar.cpp
)

target_link_libraries(
	siox-core-ComponentRegistrar
	siox-core-ActivitySerializable
)


siox_run_text_serializer(ActivitySerializable.hpp ActivitySerializableText.cpp)