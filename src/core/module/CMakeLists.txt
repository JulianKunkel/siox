project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${GLIB_INCLUDE_DIRS}
	${GMODULE2_INCLUDE_DIRS}
)

add_library(
	siox-core-ModuleLoader
	ModuleLoader.cpp
)

target_link_libraries(
	siox-core-ModuleLoader
	${GLIB_LIBRARIES}
	${GLIB_GMODULE_LIBRARIES}
	${GMODULE2_LIBRARIES}
)


add_subdirectory(test)

install(TARGETS siox-core-ModuleLoader LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
