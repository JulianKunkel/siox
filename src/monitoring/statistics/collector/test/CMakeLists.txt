project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ThreadedStatisticsCollectorTest
	ThreadedStatisticsCollectorTest.cpp
)

target_link_libraries(
	ThreadedStatisticsCollectorTest
	siox-core-ModuleLoader
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-FileOntology
	siox-monitoring-statisticsPlugin-providerskel
)

add_test(Threaded_Statistics_Collector ThreadedStatisticsCollectorTest)
