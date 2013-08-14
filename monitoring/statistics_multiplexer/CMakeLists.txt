cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${SIOX_SOURCES}/monitoring"
)

add_library(
	StatisticsMultiplexerSync SHARED
	StatisticsMultiplexerSync.cpp
)

target_link_libraries(
	StatisticsMultiplexerSync
	core-container-serializer
)

add_subdirectory(test)
