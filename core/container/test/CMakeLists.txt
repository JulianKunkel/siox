cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization)
 
include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${Boost_INCLUDE_DIR}"
)

add_executable(
	test-serializer
	test-serializer.cpp
)

target_link_libraries(
	test-serializer 
	core-container-serializer
	"${Boost_LIBRARIES}"
)
