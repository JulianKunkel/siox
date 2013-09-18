project(SIOX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmodule-2.0")

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${GLIB2_INCLUDE_DIRS}
)

add_library(
	siox-core-ModuleLoader
	ModuleLoader.cpp
)

target_link_libraries(
	siox-core-ModuleLoader
	${GLIB2_LIBRARIES}
	${GMODULE2_LIBRARIES}
)


add_subdirectory(test)
