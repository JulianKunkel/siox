project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-FileWriter
	StatisticsFileWriter.cpp
	StatisticsFileWriterOptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-FileWriter
	siox-core-AutoConfigurator
	siox-monitoring-Statistic
)

siox_run_serializer(StatisticsFileWriterOptions.hpp StatisticsFileWriterOptions.cpp)


install(TARGETS siox-monitoring-statisticsMultiplexerPlugin-FileWriter LIBRARY DESTINATION lib)


add_subdirectory(test)
