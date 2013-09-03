project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-activityPlugin-ActivityForwarder
	ActivityForwarder.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityForwarder
	siox-core-AutoConfigurator
)

add_subdirectory(test)

