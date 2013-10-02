project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	AnomalySkeletonTest
	AnomalySkeletonTest.cpp
)

target_link_libraries(
	AnomalySkeletonTest
	siox-monitoring-activityPlugin-AnomalySkeleton
	siox-core-autoconfigurator-FileConfigurationProvider
	siox-monitoring-FileOntology
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-ReasonerStandardImplementation
	siox-knowledge-DereferencingFacade
	siox-monitoring-ActivityMultiplexerAsync
)

symlink(siox.conf siox.conf)

add_test(core-activity_multiplexer-anomaly-skeleton-AnomalySkeletonTest AnomalySkeletonTest)

