project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	StatisticsTest
	StatisticsTest.cpp
)

target_link_libraries(
	StatisticsTest
	siox-core-ModuleLoader
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-FileOntology
	siox-monitoring-statisticsPlugin-providerskel
	${Boost_LIBRARIES}
)

add_test(Statistics StatisticsTest)
