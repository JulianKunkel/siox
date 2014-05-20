project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	WrapAroundTest
	WrapAroundTest.cpp
)

target_link_libraries(
	WrapAroundTest
	siox-core-ModuleLoader
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology
	siox-knowledge-DereferencingFacade
	siox-monitoring-ThreadedStatisticsCollector
	${Boost_LIBRARIES}
)

add_test(monitoring_statistics_collector_test_WrapAroundTest WrapAroundTest)
