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
	siox-monitoring-statisticsPluginGeneric
)

add_subdirectory(test)

install(TARGETS siox-monitoring-statisticsPlugin-CPUstats LIBRARY DESTINATION lib)
