project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLIntervalWriter
	StatisticsPostgreSQLIntervalWriter.cpp
	StatisticsPostgreSQLIntervalWriterOptions.cpp
)


target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLIntervalWriter
	siox-core-AutoConfigurator
	siox-monitoring-Statistic
	${Boost_LIBRARIES}
	${PostgreSQL_LIBRARY}
)

siox_run_serializer(StatisticsPostgreSQLIntervalWriterOptions.hpp StatisticsPostgreSQLIntervalWriterOptions.cpp)

install(TARGETS siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLIntervalWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

add_subdirectory(test)
