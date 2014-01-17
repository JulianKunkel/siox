project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-DatabaseTopology
	DatabaseTopology.cpp
	DatabaseTopologyOptions.cpp
)

target_link_libraries(
	siox-monitoring-DatabaseTopology
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
	pqxx
)
# TODO add pqxx detection to configure / CMAKE

siox_run_serializer(DatabaseTopologyOptions.hpp DatabaseTopologyOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-DatabaseTopology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
