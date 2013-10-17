project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ActivityPostgreSQLWriterTest
	ActivityPostgreSQLWriterTest.cpp
)

target_link_libraries(
	ActivityPostgreSQLWriterTest
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityPostgreSQLWriter
)

add_test(core-activity_multiplexer-postgresqlwriter-ActivityPostgreSQLWriterTest ActivityPostgreSQLWriterTest)

