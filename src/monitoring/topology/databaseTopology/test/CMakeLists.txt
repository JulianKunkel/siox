project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring_topology_databaseTopology_Test
	test.cpp
)

target_link_libraries(
	monitoring_topology_databaseTopology_Test
	siox-core-ModuleLoader
	siox-monitoring-DatabaseTopology
)

add_test(monitoring_topology_databaseTopology_Test monitoring_topology_databaseTopology_Test)
