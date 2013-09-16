project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	optimizer-test
	optimizer-test.cpp
)

target_link_libraries(
	optimizer-test
	siox-knowledge-OptimizerStandardImplementation
	siox-core-ModuleLoader 
	siox-monitoring-FileOntology 
)

add_test(Optimizer optimizer-test)

