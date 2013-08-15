cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization REQUIRED)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${Boost_INCLUDE_DIR}"
)

add_library(
	FileOntology
	FileOntology.cpp
)

target_link_libraries(
	FileOntology 
	core-component-registrar
	"${Boost_LIBRARIES}"
)

add_subdirectory(test)
