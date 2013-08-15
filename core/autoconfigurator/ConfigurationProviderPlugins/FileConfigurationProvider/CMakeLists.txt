cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	FileConfigurationProvider SHARED
	FileConfigurationProvider.cpp
)

add_subdirectory(test)
