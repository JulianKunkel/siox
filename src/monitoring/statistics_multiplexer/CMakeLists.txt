project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-StatisticsMultiplexerSync
	StatisticsMultiplexerSync.cpp
)

target_link_libraries(
	siox-monitoring-StatisticsMultiplexerSync
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator
)

add_subdirectory(test)

install(TARGETS siox-monitoring-StatisticsMultiplexerSync LIBRARY DESTINATION lib)
