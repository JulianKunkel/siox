cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_executable(
	siox-ll-c-test
	siox-ll-c-test.c
)

target_link_libraries(
	siox-ll-c-test
	siox-ll
	ActivityBuilder
	FileConfigurationProvider
)

