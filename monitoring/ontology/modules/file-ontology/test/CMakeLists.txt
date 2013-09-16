project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	file-ontology-test
	file-ontology-test.cpp
)

target_link_libraries(
	file-ontology-test
	siox-core-ModuleLoader 
	siox-monitoring-FileOntology
)

add_test(File-Ontology file-ontology-test)
