cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(Boost 1.53.0 COMPONENTS serialization)
find_package(GLIB2 REQUIRED)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${Boost_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmodule-2.0")

add_executable(
	test-fileconfiguration
	fileconfiguration-test.cpp
)

target_link_libraries(
	test-fileconfiguration
	test-serialize-modules
)

add_library(
	test-serialize-modules SHARED
	test-serialize-modules.cpp
)

target_link_libraries(
	test-serialize-modules 
	AutoConfigurator
	FileConfigurationProvider
	core-component-registrar
	core-container-serializer
	module-loader
	"${Boost_LIBRARIES}"
	"${GLIB2_LIBRARIES}"
)

