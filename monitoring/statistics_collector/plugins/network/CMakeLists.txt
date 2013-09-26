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
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-monitoring-statisticsPlugin-network LIBRARY DESTINATION lib)
