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
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-core-ContainerSerializer LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
