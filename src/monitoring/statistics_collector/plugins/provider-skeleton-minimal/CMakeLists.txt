project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsPlugin-providerskel-min
	iostats.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-providerskel-min
	siox-monitoring-statisticsPluginGeneric
)

add_subdirectory(test)


