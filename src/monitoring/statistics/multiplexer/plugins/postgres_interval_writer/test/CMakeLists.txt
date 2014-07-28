project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-statistics-multiplexer-plugins-postgres_interval_writer-test
	StatisticsPostgreSQLIntervalWriterTest.cpp
)

target_link_libraries(
	core-statistics-multiplexer-plugins-postgres_interval_writer-test
	siox-core-ModuleLoader 	
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsPlugin-OSMemUsage
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology	
)

#target_compile_options(
#	core-statistics-multiplexer-plugins-postgres_interval_writer-test
#	PUBLIC
#	-DTEST_STUB
#)


add_test(core-statistics-multiplexer-plugins-postgres_interval_writer core-statistics-multiplexer-plugins-postgres_interval_writer-test)

