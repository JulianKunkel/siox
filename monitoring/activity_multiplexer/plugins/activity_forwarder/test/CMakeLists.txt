cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SIOX_INCLUDE_DIR}/cmake/cmake-glib2")

find_package(Boost 1.53.0 COMPONENTS serialization)
find_package(GLIB2 REQUIRED)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${SIOX_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIR}"
	"${GLIB2_INCLUDE_DIRS}"
)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -pthread -lrt -lgmodule-2.0")

# add_executable(
# 	AnomalySkeletonTest
# 	AnomalySkeletonTest.cpp
# )
# 
# target_link_libraries(
# 	AnomalySkeletonTest
# 	AnomalySkeleton
# 	AutoConfigurator
# 	FileConfigurationProvider
# 	core-component-registrar
# 	core-container-serializer
# 	module-loader
# 	"${Boost_LIBRARIES}"
# 	${GLIB2_LIBRARIES}
# 	${LINK_LIBRARIES}
# )

