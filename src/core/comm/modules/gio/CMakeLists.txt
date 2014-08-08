project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${GLIB_INCLUDE_DIRS}
)

add_library(
	siox-core-comm-gio
	GIO.cpp 
	GIOClient.cpp 
	GIOServer.cpp 
	GIOOptions.cpp
)

target_link_libraries(
	siox-core-comm-gio
	siox-util-jobProcessors
	siox-util
	${GLIB_LIBRARIES}
        ${GLIB_GOBJECT_LIBRARIES}
        ${GLIB_GMODULE_LIBRARIES}
        ${GLIB_GIO_LIBRARIES}
	${GIOUNIX2_LIBRARIES}
	${Boost_LIBRARIES}
	siox-core-comm
)

siox_run_serializer(GIOOptions.hpp GIOOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-core-comm-gio LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
