project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/monitoring/
)

add_library(
	siox-monitoring-ActivityBuilder
	ActivityBuilder.cpp
)

target_link_libraries(
	siox-monitoring-ActivityBuilder
	siox-util
	rt
)

add_subdirectory(test)

install(TARGETS siox-monitoring-ActivityBuilder LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
