project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${GLIB_INCLUDE_DIRS}
)

add_library(
	siox-core-ModuleLoader
	ModuleLoader.cpp
)

target_link_libraries(
	siox-core-ModuleLoader
	${GLIB_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-core-ModuleLoader LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
