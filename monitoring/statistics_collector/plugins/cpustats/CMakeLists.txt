project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPlugin-CPUstats
	cpustats.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-CPUstats 
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_subdirectory(test)
