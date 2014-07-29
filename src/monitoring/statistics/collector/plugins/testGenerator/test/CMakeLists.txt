project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring_statistics_collector_plugins_testGenerator
	testGeneratorTest.cpp
)

target_link_libraries(
	monitoring_statistics_collector_plugins_testGenerator
	siox-monitoring-statisticsPlugin-testGenerator 
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_test(monitoring_statistics_collector_plugins_testGenerator monitoring_statistics_collector_plugins_testGenerator)


