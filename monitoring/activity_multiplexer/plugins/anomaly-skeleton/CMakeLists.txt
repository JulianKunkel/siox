cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	AnomalySkeleton SHARED
	AnomalySkeleton.cpp
)

target_link_libraries(
	AnomalySkeleton 
	AutoConfigurator
)

add_subdirectory(test)

