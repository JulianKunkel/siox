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
	siox-core-AutoConfigurator
)

add_subdirectory(test)
