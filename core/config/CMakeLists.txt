cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-yaml/")

find_package(YamlCpp REQUIRED)

include_directories(
	"${CMAKE_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/logger"
	"${PROJECT_SOURCE_DIR}"
	"${YAMLCPP_INCLUDE_DIR}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11")

add_library(sioxconf
	Configuration.h
	Configuration.cpp
)

target_link_libraries(sioxconf ${LINK_LIBRARIES} ${YAMLCPP_LIBRARY})
