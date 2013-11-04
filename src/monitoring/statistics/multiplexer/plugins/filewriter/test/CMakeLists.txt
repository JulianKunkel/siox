project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	StatisticsFileWriterTest
	StatisticsFileWriterTest.cpp
)

target_link_libraries(
	StatisticsFileWriterTest
	siox-core-ModuleLoader 	
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsMultiplexerPlugin-FileWriter
)



add_test(core-statistics-multiplexer-filewriter-StatisticsFileWriterTest StatisticsFileWriterTest)

