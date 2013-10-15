project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring-low-level-c-test
	siox-ll-c-test.c
)

target_link_libraries(
	monitoring-low-level-c-test
	siox-monitoring-siox-ll 
	siox-monitoring-activityPlugin-AnomalySkeleton
	siox-core-autoconfigurator-FileConfigurationProvider
	siox-monitoring-FileOntology
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-ReasonerStandardImplementation
	siox-knowledge-DereferencingFacade
	siox-monitoring-ActivityMultiplexerAsync	
	siox-monitoring-activityPlugin-ActivityForwarder
)

symlink(siox.conf siox.conf)

add_test(monitoring-low-level-c-test monitoring-low-level-c-test)
