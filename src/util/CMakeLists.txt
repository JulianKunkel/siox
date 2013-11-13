project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-util
	autoLoadModules.cpp
	ReporterHelper.cpp
	time.c
	threadSafety.c
)

target_link_libraries(
	siox-util
	siox-core-AutoConfigurator
)

install(TARGETS siox-util LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL(FILES ${files} DESTINATION include/util)


add_subdirectory(JobProcessors)
