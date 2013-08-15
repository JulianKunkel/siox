cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_library(
	mufs
	mufs.c
)

target_link_libraries(
	mufs
	siox-ll
	FileConfigurationProvider
)

add_executable(
	mockupfs-example
	mockupfs-example.c
)

target_link_libraries(
	mockupfs-example
	mufs
	siox-ll
	ActivityBuilder
)

