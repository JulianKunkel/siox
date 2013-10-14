project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPlugin-IOstats
	iostats.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-IOstats
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-monitoring-statisticsPlugin-IOstats LIBRARY DESTINATION lib)
