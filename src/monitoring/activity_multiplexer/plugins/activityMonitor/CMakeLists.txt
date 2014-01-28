project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityMonitor
	ActivityMonitor.cpp
	ActivityMonitorOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityMonitor 
	siox-core-AutoConfigurator
)

siox_run_serializer(ActivityMonitorOptions.hpp ActivityMonitorOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityMonitor LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
