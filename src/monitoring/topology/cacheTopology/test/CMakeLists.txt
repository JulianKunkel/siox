project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PQXX_INCLUDE_DIRS}
)

add_executable(
	monitoring_topology_cacheTopology_Test
	test.cpp
)

target_link_libraries(
	monitoring_topology_cacheTopology_Test
	siox-core-ModuleLoader
	siox-monitoring-cacheTopology
	${PQXX_LIBRARIES}
	${PostgreSQL_LIBRARY}
)

add_test(monitoring_topology_cacheTopology_Test monitoring_topology_cacheTopology_Test)
