cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_executable(
	iostatsTest
	iostatsTest.cpp
)

target_link_libraries(
	iostatsTest
	iostats 
	AutoConfigurator
)
