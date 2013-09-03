project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRs}
)

add_library(
	siox-monitoring-statisticsPlugin-providerskel
	ProviderSkeleton.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-providerskel
	siox-core-AutoConfigurator
)

add_subdirectory(test)
