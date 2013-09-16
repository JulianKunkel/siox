project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	iostatsTestSkel
	iostatsTest.cpp
)

target_link_libraries(
	iostatsTestSkel
	siox-monitoring-statisticsPlugin-IOstats 
	siox-core-AutoConfigurator
)

add_test(Provider-Skeleton-Minimal iostatsTestSkel)
