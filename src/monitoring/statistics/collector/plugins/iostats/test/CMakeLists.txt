project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	iostatsTest
	iostatsTest.cpp
)

target_link_libraries(
	iostatsTest
	siox-monitoring-statisticsPlugin-IOstats 
	siox-core-AutoConfigurator
)

add_test(IO-Statistics iostatsTest)