project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-ThreadedStatisticsCollector
	ThreadedStatisticsCollector.cpp
	Statistic.cpp
	StatisticsCollection.cpp
	ThreadedStatisticsOptions.cpp
)

target_link_libraries(
	siox-monitoring-ThreadedStatisticsCollector
	siox-core-AutoConfigurator
	siox-util
	siox-monitoring-Topology
	${Boost_LIBRARIES}
)

siox_run_serializer(ThreadedStatisticsOptions.hpp ThreadedStatisticsOptions.cpp)

add_subdirectory(plugins)
add_subdirectory(test)

install(TARGETS siox-monitoring-ThreadedStatisticsCollector LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
