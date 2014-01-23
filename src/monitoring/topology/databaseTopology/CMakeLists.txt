project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${libpqxx_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-DatabaseTopology
	DatabaseTopology.cpp
	DatabaseTopologyOptions.cpp
)

target_link_libraries(
	siox-monitoring-DatabaseTopology
	siox-core-AutoConfigurator
	siox-monitoring-Topology
	${Boost_LIBRARIES}
	${libpqxx_LDFLAGS}
	pq
)

siox_run_serializer(DatabaseTopologyOptions.hpp DatabaseTopologyOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-DatabaseTopology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
