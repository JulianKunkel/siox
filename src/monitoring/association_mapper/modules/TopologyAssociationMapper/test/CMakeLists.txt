project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring-association_mapper-TopologyAssociationMapper
	test.cpp
)

target_link_libraries(
	monitoring-association_mapper-TopologyAssociationMapper
	siox-core-ModuleLoader 
	siox-monitoring-TopologyAssociationMapper
	siox-monitoring-RamTopology
)

add_test(monitoring-association_mapper-TopologyAssociationMapper monitoring-association_mapper-TopologyAssociationMapper)

