cmake_minimum_required(VERSION 2.8)
project(SIOX)

find_package(GLIB2 REQUIRED)
find_package(Boost 1.53.0 COMPONENTS serialization)

include_directories(
	"${SIOX_INCLUDE}"
	"${SIOX_SOURCES}"
	"${Boost_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmodule-2.0")

add_executable(
	AnomalySkeletonTest
	AnomalySkeletonTest.cpp
)

target_link_libraries(
	AnomalySkeletonTest
	AnomalySkeleton
	AutoConfigurator
	FileConfigurationProvider
	core-component-registrar
	core-container-serializer
	module-loader
	"${Boost_LIBRARIES}"
	"${GLIB2_LIBRARIES}"
)

