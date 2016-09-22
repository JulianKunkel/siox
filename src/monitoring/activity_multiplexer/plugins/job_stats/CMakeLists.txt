project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	#${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-JobStatsPlugin
	JobStats.cpp
	JobStatsOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-JobStatsPlugin
	siox-core-ModuleLoader
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(JobStatsOptions.hpp JobStatsOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-JobStatsPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

