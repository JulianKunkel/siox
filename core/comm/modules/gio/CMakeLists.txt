project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${GLIB2_INCLUDE_DIRS}
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
)
siox_run_serializer(GIOOptions.hpp BoostXMLSerializable.cpp)

# add_subdirectory(test)
