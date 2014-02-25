project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-statistics-multiplexer-filewriter-statisticsHealthADPI
	Test.cpp
)

target_link_libraries(
	core-statistics-multiplexer-filewriter-statisticsHealthADPI
	siox-core-ModuleLoader 	
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsMultiplexerPlugin-healthADPI
)



add_test(core-statistics-multiplexer-filewriter-statisticsHealthADPI core-statistics-multiplexer-filewriter-statisticsHealthADPI)

