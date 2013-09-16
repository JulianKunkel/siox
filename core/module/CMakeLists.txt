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

add_subdirectory(test)
