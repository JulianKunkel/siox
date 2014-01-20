project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-monitoring-activity_multiplexer-fadviseReadAhead-test
	test.cpp
)

target_link_libraries(
	core-monitoring-activity_multiplexer-fadviseReadAhead-test
	siox-monitoring-activityPlugin-fadviseReadAhead
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-DereferencingFacade
	siox-core-reporting-ConsoleReporter
	siox-monitoring-RamTopology
	siox-monitoring-FileOntology
)

add_test(core-monitoring-activity_multiplexer-fadviseReadAhead-test core-monitoring-activity_multiplexer-fadviseReadAhead-test)

