project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	RamTopologyTest
	RamTopologyTest.cpp
)

target_link_libraries(
	RamTopologyTest
	siox-core-ModuleLoader

	siox-monitoring-RamTopology
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

add_test(monitoring_topology_RamTopologyTest RamTopologyTest)
