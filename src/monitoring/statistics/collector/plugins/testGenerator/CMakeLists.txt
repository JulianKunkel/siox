project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPlugin-testGenerator
	TestGenerator.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-testGenerator 
	siox-monitoring-statisticsPluginGeneric
)

add_subdirectory(test)

siox_run_serializer(testGeneratorOptions.hpp testGeneratorOptions.cpp)


install(TARGETS siox-monitoring-statisticsPlugin-testGenerator LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
