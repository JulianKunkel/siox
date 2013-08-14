cmake_minimum_required(VERSION 2.8)
project(SIOX)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
)

add_executable(
	iostatsTestSkel
	iostatsTest.cpp
)

target_link_libraries(
	iostatsTestSkel
	StatisticsProviderSkeleton
	AutoConfigurator
	module-loader
)
