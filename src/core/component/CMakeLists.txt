project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-core-ActivitySerializable
	ActivitySerializableText.cpp
)

add_library(
	siox-core-ComponentRegistrar
	ComponentRegistrar.cpp
)

target_link_libraries(
	siox-core-ActivitySerializable
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)


target_link_libraries(
	siox-core-ComponentRegistrar
	siox-core-ActivitySerializable
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)


siox_run_text_serializer(ActivitySerializable.hpp ActivitySerializableText.cpp)

install(TARGETS siox-core-ComponentRegistrar siox-core-ActivitySerializable LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
