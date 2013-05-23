cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS system thread unit_test_framework REQUIRED)

include_directories(
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/comm"
	"${CMAKE_SOURCE_DIR}/logger"
	"${CMAKE_SOURCE_DIR}/config"
	"${CMAKE_SOURCE_DIR}/comm/build"
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lboost_system -lboost_thread -lboost_unit_test_framework")

add_executable(communication_tests
	communication.cc)

target_link_libraries(communication_tests sioxcomm)

add_executable(configuration_tests
	configuration.cc)

target_link_libraries(configuration_tests sioxconf sioxlog)
