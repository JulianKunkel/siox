project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${GLIB2_INCLUDE_DIRS}
	${GIOUNIX2_INCLUDE_DIRS}
)

add_library(
	siox-core-comm-gio
	GIO.cpp 
	GIOClient.cpp 
	GIOServer.cpp 
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-core-comm-gio
	${Boost_LIBRARIES}
	${GLIB2_LIBRARIES}
	${GIOUNIX2_LIBRARIES}
)
siox_run_serializer(GIOOptions.hpp BoostXMLSerializable.cpp)

add_subdirectory(test)

install(TARGETS siox-core-comm-gio LIBRARY DESTINATION lib)
