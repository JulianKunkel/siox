project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-activity_multiplexer-ActivityMonitorTest
	ActivityMonitorTest.cpp
)

target_link_libraries(
	core-activity_multiplexer-ActivityMonitorTest
	siox-monitoring-activityPlugin-ActivityMonitor
	siox-core-autoconfigurator-FileConfigurationProvider
	siox-monitoring-FileOntology
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-ReasonerStandardImplementation
	siox-knowledge-DereferencingFacade
	siox-monitoring-ActivityMultiplexerAsync
)

symlink(siox.conf siox.conf)

add_test(core-activity_multiplexer-ActivityMonitorTest core-activity_multiplexer-ActivityMonitorTest)

