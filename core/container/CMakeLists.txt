project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-core-ContainerSerializer
	container-serializer.cpp
)

add_subdirectory(test)

