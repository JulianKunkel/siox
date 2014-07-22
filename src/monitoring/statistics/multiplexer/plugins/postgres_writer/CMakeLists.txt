project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLWriter
	StatisticsPostgreSQLWriter.cpp
	StatisticsPostgreSQLWriterOptions.cpp
)


target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLWriter
	siox-core-AutoConfigurator
	siox-monitoring-Statistic
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
	${PostgreSQL_LIBRARY}
)

siox_run_serializer(StatisticsPostgreSQLWriterOptions.hpp StatisticsPostgreSQLWriterOptions.cpp)

install(TARGETS siox-monitoring-statisticsMultiplexerPlugin-PostgreSQLWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

