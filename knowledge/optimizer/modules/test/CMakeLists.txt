cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_executable(
	optimizer-test
	optimizer-test.cpp
)

target_link_libraries(
	optimizer-test
	OptimizerStandardImplementation
	module-loader
	FileOntology
)

