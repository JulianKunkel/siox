cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${SIOX_SOURCES}/monitoring"
	"${SIOX_SOURCES}/knowledge/activity_plugin/"
)

add_library(
	ActivityMultiplexerAsyncThreaded
	ActivityMultiplexerAsyncThreaded.cpp
	ActivityPluginDereferencingStandardImpl.cpp
)

add_subdirectory(plugins)

#add_subdirectory(test)

