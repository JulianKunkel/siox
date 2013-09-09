project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-ActivityMultiplexerAsync
	ActivityMultiplexerAsync.cpp
)

target_link_libraries(
	siox-monitoring-ActivityMultiplexerAsync
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator 
)

#add_subdirectory(plugins)
#add_subdirectory(test)

