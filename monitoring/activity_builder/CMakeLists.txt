cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${SIOX_SOURCES}/monitoring/
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_library(
	ActivityBuilder
	ActivityBuilder.cpp
	${SIOX_SOURCES}/monitoring/helpers/siox_time.c
)

add_subdirectory(test)
