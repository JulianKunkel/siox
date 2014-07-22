project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	WrapAroundTestDefault
	WrapAroundTestDefault.cpp
)

target_link_libraries(
	WrapAroundTestDefault
	siox-core-ModuleLoader
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology
	siox-knowledge-DereferencingFacade
	siox-monitoring-ThreadedStatisticsCollector
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

add_test(monitoring_statistics_collector_test_WrapAroundTestDefault WrapAroundTestDefault)

add_executable(
	WrapAroundTestCustom
	WrapAroundTestCustom.cpp
)

target_link_libraries(
	WrapAroundTestCustom
	siox-core-ModuleLoader
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology
	siox-knowledge-DereferencingFacade
	siox-monitoring-ThreadedStatisticsCollector
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

add_test(monitoring_statistics_collector_test_WrapAroundTestCustom WrapAroundTestCustom)
