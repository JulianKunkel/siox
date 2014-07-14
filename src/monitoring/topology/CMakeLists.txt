project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-RamTopology
	RamTopology.cpp
	RamTopologyOptions.cpp
)

target_link_libraries(
	siox-monitoring-RamTopology
	siox-core-AutoConfigurator
	siox-util
	${Boost_LIBRARIES}
)


add_library(
	siox-monitoring-Topology
	Topology.cpp
)

target_link_libraries(
	siox-monitoring-RamTopology
	siox-core-AutoConfigurator
	siox-monitoring-Topology
	siox-util
	${Boost_LIBRARIES}
)

siox_run_serializer(RamTopologyOptions.hpp RamTopologyOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-RamTopology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
install(TARGETS siox-monitoring-Topology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

if (PostgreSQL_FOUND)
	add_subdirectory(databaseTopology)
	add_subdirectory(cacheTopology)
endif (PostgreSQL_FOUND)

add_subdirectory(NetworkForwarderTopology)
