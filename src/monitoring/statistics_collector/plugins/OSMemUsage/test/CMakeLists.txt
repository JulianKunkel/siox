project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	OSMemUsageTest
	OSMemUsageTest.cpp
)

target_link_libraries(
	OSMemUsageTest
	siox-monitoring-statisticsPlugin-OSMemUsage 
	siox-core-AutoConfigurator
)

add_test(Memory-Statistics OSMemUsageTest)
