cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	ActivityFileWriter SHARED
	ActivityFileWriter.cpp
)

target_link_libraries(
	ActivityFileWriter
	core-container-serializer 
	AutoConfigurator 
)

add_subdirectory(test)

