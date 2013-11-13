project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPlugin-network
	network.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-network 
	siox-monitoring-statisticsPluginGeneric
)

add_subdirectory(test)

install(TARGETS siox-monitoring-statisticsPlugin-network LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
