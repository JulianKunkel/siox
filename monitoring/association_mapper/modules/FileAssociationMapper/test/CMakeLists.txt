cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SIOX_INCLUDE_DIR}/cmake/cmake-glib2")

find_package(GLIB2 REQUIRED)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"

)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt")

add_executable(
	file-associationmapper-test
	file-associationmapper-test.cpp
)

target_link_libraries(
	file-associationmapper-test
	FileAssociationMapper
	module-loader
	"${Boost_LIBRARIES}"
	"${GLIB2_LIBRARIES}"
	"${LINK_LIBRARIES}"
)


