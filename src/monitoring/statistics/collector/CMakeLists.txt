project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-ThreadedStatisticsCollector
	ThreadedStatisticsCollector.cpp
	Statistic.cpp
)

target_link_libraries(
	siox-monitoring-ThreadedStatisticsCollector
	${Boost_LIBRARIES}
)

siox_run_serializer(ThreadedStatisticsOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(plugins)
add_subdirectory(test)

install(TARGETS siox-monitoring-ThreadedStatisticsCollector LIBRARY DESTINATION lib)
