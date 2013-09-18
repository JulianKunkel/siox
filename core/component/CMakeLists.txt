project(SIOX)

include_directories(${SIOX_INCLUDE_DIRS})

add_library(
	siox-core-ComponentRegistrar
	ComponentRegistrar.cpp
	ActivitySerializableText.hpp
)

siox_run_text_serializer(ActivitySerializable.hpp ActivitySerializableText.hpp)