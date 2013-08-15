cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	MathematicalEquation
	MathematicalEquation.cpp
)

add_subdirectory(test)
