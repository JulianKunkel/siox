project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${GLIB_INCLUDE_DIRS}
	${GIOUNIX2_INCLUDE_DIRS}
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
	${Boost_LIBRARIES}
	${GLIB_LIBRARIES}
        ${GLIB_GOBJECT_LIBRARIES}
        ${GLIB_GMODULE_LIBRARIES}
        ${GLIB_GIO_LIBRARIES}
	${GIOUNIX2_LIBRARIES}
	siox-core-comm
)
siox_run_serializer(GIOOptions.hpp GIOOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-core-comm-gio LIBRARY DESTINATION lib)
