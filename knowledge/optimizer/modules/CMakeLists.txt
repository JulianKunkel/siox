project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-knowledge-OptimizerStandardImplementation
	OptimizerStandardImplementation.cpp
)

target_link_libraries(
	siox-knowledge-OptimizerStandardImplementation
	siox-core-AutoConfigurator
)

add_subdirectory(test)
