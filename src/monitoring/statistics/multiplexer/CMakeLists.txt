project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-StatisticsMultiplexerSync
	StatisticsMultiplexerSync.cpp
	StatisticsMultiplexerSyncOptions.cpp
)

target_link_libraries(
	siox-monitoring-StatisticsMultiplexerSync
	siox-core-AutoConfigurator
	siox-util
)

add_subdirectory(plugins)

siox_run_serializer(StatisticsMultiplexerSyncOptions.hpp StatisticsMultiplexerSyncOptions.cpp)

install(TARGETS siox-monitoring-StatisticsMultiplexerSync LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
