project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${GIOUNIX2_INCLUDE_DIRS}
)

add_library(
	siox-core-comm
	CommunicationModule.cpp
)

target_link_libraries(
	siox-core-comm
	siox-util-jobProcessors
)

install(TARGETS siox-core-comm LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


add_subdirectory(modules)
add_subdirectory(test)

