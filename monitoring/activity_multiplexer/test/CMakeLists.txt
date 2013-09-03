project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	anomaly-plugin
	AnomalyPlugin.cpp
)

target_link_libraries(
	anomaly-plugin
	siox-monitoring-ActivityMultiplexerAsyncThreaded 
	siox-core-ModuleLoader 
	siox-monitoring-activityPlugin-ActivityFileWriter
)

add_test(Anomaly-Plugin anomaly-plugin)
