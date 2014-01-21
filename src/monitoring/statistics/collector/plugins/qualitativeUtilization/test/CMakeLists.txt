project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	QualitativeUtilizationTest
	QualitativeUtilizationTest.cpp
)

target_link_libraries(
	QualitativeUtilizationTest
	siox-core-AutoConfigurator

	siox-monitoring-statisticsPlugin-qualitativeUtilization
	siox-monitoring-RamTopology
	siox-knowledge-DereferencingFacade
	siox-monitoring-FileOntology
	siox-monitoring-statisticsPlugin-CPUstats
	siox-monitoring-statisticsPlugin-network
	siox-monitoring-statisticsPlugin-IOstats
	siox-monitoring-statisticsPlugin-OSMemUsage
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsPlugin-qualitativeUtilization
)

add_test(monitoring_statistics_collector_plugins_qualitativeUtilization_test QualitativeUtilizationTest)
