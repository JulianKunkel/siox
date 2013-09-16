project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-activityPlugin-AnomalySkeleton
	AnomalySkeleton.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-AnomalySkeleton 
	siox-core-AutoConfigurator
)

add_subdirectory(test)

