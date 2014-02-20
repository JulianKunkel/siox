project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${LIKWID_INCLUDE_DIRS}
)

add_executable(
	monitoring-statistics-collector-plugin-likwid-test
	test.cpp
)

target_link_libraries(
	monitoring-statistics-collector-plugin-likwid-test
	siox-monitoring-statisticsPlugin-likwid 
	siox-core-AutoConfigurator
	${LIKWID_LIBRARIES}
)

add_test(monitoring-statistics-collector-plugin-likwid-test monitoring-statistics-collector-plugin-likwid-test)
