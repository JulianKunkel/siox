project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring-ontology-TopologyOntology-Test
	test.cpp
)

target_link_libraries(
	monitoring-ontology-TopologyOntology-Test
	siox-core-ModuleLoader 
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology
)

add_test(monitoring-ontology-TopologyOntology-Test monitoring-ontology-TopologyOntology-Test)
