project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	test-mufs
	mufs.c
)

target_link_libraries(
	test-mufs
	siox-monitoring-siox-ll
)

add_executable(
	mockupfs-example
	mockupfs-example.c
)

target_link_libraries(
	mockupfs-example
	test-mufs 
	siox-monitoring-siox-ll 
	siox-core-autoconfigurator-FileConfigurationProvider
	siox-monitoring-activityPlugin-AnomalySkeleton
	siox-monitoring-FileOntology
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-ReasonerStandardImplementation
	siox-knowledge-DereferencingFacade
	siox-monitoring-ActivityMultiplexerAsync	
	siox-monitoring-activityPlugin-ActivityForwarder	
)

symlink(siox.conf siox.conf)

add_test(monitoring-low-level-c-examples-mockupfs-example mockupfs-example)