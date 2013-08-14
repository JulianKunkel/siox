cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${SIOX_INCLUDE}/monitoring/helpers"
)

add_executable(
	anomaly-plugin
	AnomalyPlugin.cpp
)

target_link_libraries(
	anomaly-plugin
	module-loader
	ActivityMultiplexerAsyncThreaded
)

add_executable(
	anomaly-plugin-threaded
	AnomalyPluginThreaded.cpp
)

target_link_libraries(
	anomaly-plugin-threaded
	module-loader
	ActivityMultiplexerAsyncThreaded
)
