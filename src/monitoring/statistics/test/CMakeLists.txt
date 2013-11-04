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
	siox-monitoring-FileOntology

	siox-monitoring-statisticsPlugin-providerskel
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsMultiplexerPlugin-testListener
	siox-monitoring-statisticsMultiplexerPlugin-FileWriter
	${Boost_LIBRARIES}
)

add_test(monitoring_StatisticsTest StatisticsTest)
