project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	monitoring-system_information-modules-TopologySystemInformation
	test.cpp
)

target_link_libraries(
	monitoring-system_information-modules-TopologySystemInformation
	siox-core-ModuleLoader 
	siox-monitoring-TopologySystemInformation
	siox-monitoring-RamTopology
)

add_test(monitoring-system_information-modules-TopologySystemInformation monitoring-system_information-modules-TopologySystemInformation)

