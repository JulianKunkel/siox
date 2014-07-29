project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIR}
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
	siox-monitoring-statisticsPlugin-testGenerator
	siox-monitoring-TopologyOntology
	siox-monitoring-RamTopology
	${PostgreSQL_LIBRARY}
)

#target_compile_options(
#	core-statistics-multiplexer-plugins-postgres_interval_writer-test
#	PUBLIC
#	-DTEST_STUB
#)


add_test(core-statistics-multiplexer-plugins-postgres_interval_writer core-statistics-multiplexer-plugins-postgres_interval_writer-test)

