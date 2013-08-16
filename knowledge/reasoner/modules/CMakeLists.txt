cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization REQUIRED)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_library(
	ReasonerStandardImplementation
	ReasonerStandardImplementation.cpp
	${Boost_INCLUDE_DIR}
)

target_link_libraries(
	ReasonerStandardImplementation
	module-loader
	${Boost_LIBRARIES}
)

add_subdirectory(test)
