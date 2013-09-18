project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/monitoring/
)

add_library(
	siox-monitoring-ActivityBuilder
	ActivityBuilder.cpp
	${SIOX_SOURCES}/monitoring/helpers/siox_time.c
)

target_link_libraries(
	siox-monitoring-ActivityBuilder
	rt
)

add_subdirectory(test)
