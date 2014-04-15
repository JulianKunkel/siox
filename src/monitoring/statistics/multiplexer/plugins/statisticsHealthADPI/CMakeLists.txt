project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-healthADPI
	StatisticsHealthADPI.cpp
	StatisticsHealthADPIOptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-healthADPI
	siox-core-AutoConfigurator
	siox-monitoring-Statistic
)

siox_run_serializer(StatisticsHealthADPIOptions.hpp StatisticsHealthADPIOptions.cpp)


install(TARGETS siox-monitoring-statisticsMultiplexerPlugin-statisticsHealthADPI LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


add_subdirectory(test)
