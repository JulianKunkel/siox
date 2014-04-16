project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-statistics-multiplexer-statisticsHealthADPI-statisticsHealthAdpiTest
	Test.cpp
)

target_link_libraries(
	core-statistics-multiplexer-statisticsHealthADPI-statisticsHealthAdpiTest
	siox-core-ModuleLoader
	siox-monitoring-ThreadedStatisticsCollector
	siox-monitoring-StatisticsMultiplexerSync
	siox-monitoring-statisticsMultiplexerPlugin-statisticsHealthADPI
)

# symlink(siox.conf siox.conf)


add_test(core-statistics-multiplexer-statisticsHealthADPI-statisticsHealthAdpiTest core-statistics-multiplexer-statisticsHealthADPI-statisticsHealthAdpiTest)

