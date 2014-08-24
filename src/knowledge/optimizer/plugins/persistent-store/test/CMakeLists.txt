project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	knowledge_optimizer_plugins_persistent-store-test
	test.cpp
)

target_link_libraries(
	knowledge_optimizer_plugins_persistent-store-test
	siox-knowledge-OptimizerStandardImplementation
	siox-knowledge-optimizer-persistent-store
	siox-core-ModuleLoader 
	siox-monitoring-FileOntology
)

add_test(knowledge_optimizer_plugins_persistent-store-test knowledge_optimizer_plugins_persistent-store-test)

