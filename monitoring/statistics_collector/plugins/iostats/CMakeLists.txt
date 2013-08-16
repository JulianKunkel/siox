cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS regex serialization REQUIRED)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)

add_library(
	iostats
	iostats.cpp
)

target_link_libraries(
	iostats
	AutoConfigurator
	module-loader
	${Boost_LIBRARIES}
)

add_subdirectory(test)
