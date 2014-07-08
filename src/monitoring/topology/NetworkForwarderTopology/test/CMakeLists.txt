project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PQXX_INCLUDE_DIRS}
)

add_executable(
	monitoring_topology_NetworkForwarderTopology_Test
	test.cpp
)

target_link_libraries(
	monitoring_topology_NetworkForwarderTopology_Test
	siox-core-ModuleLoader
	siox-monitoring-NetworkForwarderTopology
	${PQXX_LIBRARIES}	
	pq
)

add_test(monitoring_topology_NetworkForwarderTopology_Test monitoring_topology_NetworkForwarderTopology_Test)
