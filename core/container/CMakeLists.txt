project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-core-ContainerSerializer
	container-serializer.cpp
)

target_link_libraries(
	siox-core-ContainerSerializer
	${Boost_LIBRARIES}
)

add_subdirectory(test)

