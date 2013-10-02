project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsPlugin-OSMemUsage
	OSMemUsage.cpp
)

target_link_libraries(
	siox-monitoring-statisticsPlugin-OSMemUsage
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS siox-monitoring-statisticsPlugin-OSMemUsage LIBRARY DESTINATION lib)
