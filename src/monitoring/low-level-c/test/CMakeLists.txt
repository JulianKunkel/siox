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
	siox-monitoring-RamTopology
	siox-monitoring-FileOntology
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-ReasonerStandardImplementation
	siox-knowledge-DereferencingFacade
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityForwarder
	siox-knowledge-OptimizerStandardImplementation
)

symlink(siox.conf siox.conf)

add_test(monitoring-low-level-c-test monitoring-low-level-c-test)


siox_skeleton_builder(plain ${CMAKE_SOURCE_DIR}/src/include/C/siox-ll.h ${CMAKE_SOURCE_DIR}/tools/siox-skeleton-builder/stubs/null/template-null.py SIOX siox-ll-null.c)

add_library(
	siox-monitoring-siox-ll-null
	STATIC
	siox-ll-null.c
)

