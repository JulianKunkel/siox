cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_executable(
	file-ontology-test
	file-ontology-test.cpp
)

target_link_libraries(
	file-ontology-test
	FileOntology 
	module-loader
)

