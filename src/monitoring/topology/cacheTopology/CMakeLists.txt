project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${PQXX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-cacheTopology
	cacheTopology.cpp
	cacheTopologyOptions.cpp
)

target_link_libraries(
	siox-monitoring-cacheTopology
	siox-core-AutoConfigurator
	siox-monitoring-Topology
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
	${PQXX_LIBRARIES}	
	${PostgreSQL_LIBRARY}
)


siox_run_serializer(cacheTopologyOptions.hpp cacheTopologyOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-cacheTopology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
