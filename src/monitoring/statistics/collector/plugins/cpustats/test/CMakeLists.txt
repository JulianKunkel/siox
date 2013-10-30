project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	cpustatsTest
	cpustatsTest.cpp
)

target_link_libraries(
	cpustatsTest
	siox-monitoring-statisticsPlugin-CPUstats 
	siox-core-AutoConfigurator
)

add_test(CPU-Statistics cpustatsTest)


