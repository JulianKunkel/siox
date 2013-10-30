project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	networkTest
	networkTest.cpp
)

target_link_libraries(
	networkTest
	siox-monitoring-statisticsPlugin-network 
	siox-core-AutoConfigurator
)

add_test(Network-Statistics networkTest)
