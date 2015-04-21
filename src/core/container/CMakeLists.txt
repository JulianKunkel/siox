project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-ContainerSerializer
	container-serializer.cpp
)

target_link_libraries(
	siox-core-ContainerSerializer
)

add_subdirectory(test)

install(TARGETS siox-core-ContainerSerializer LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
