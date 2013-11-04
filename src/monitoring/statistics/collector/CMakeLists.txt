project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-ThreadedStatisticsCollector
	ThreadedStatisticsCollector.cpp
	Statistic.cpp
	ThreadedStatisticsOptions.cpp
)

target_link_libraries(
	siox-monitoring-ThreadedStatisticsCollector
	siox-core-AutoConfigurator
	siox-util
	${Boost_LIBRARIES}
)

siox_run_serializer(ThreadedStatisticsOptions.hpp ThreadedStatisticsOptions.cpp)


add_subdirectory(plugins)

install(TARGETS siox-monitoring-ThreadedStatisticsCollector LIBRARY DESTINATION lib)
